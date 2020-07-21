#include "compiler/pipes/register-kphp-configuration.h"

#include <re2/re2.h>

#include "compiler/compiler-core.h"
#include "compiler/data/class-data.h"
#include "compiler/data/function-data.h"
#include "compiler/gentree.h"

bool RegisterKphpConfiguration::check_function(FunctionPtr function) const {
  return function->type == FunctionData::func_class_holder &&
         function->class_id->name == configuration_class_name_;
}

void RegisterKphpConfiguration::on_start() {
  const auto klass = current_function->class_id;
  kphp_error(
    !klass->members.has_any_instance_method() &&
    !klass->members.has_any_instance_var() &&
    !klass->members.has_any_static_method() &&
    !klass->members.has_any_static_var(),
    fmt_format("{} class must be empty", configuration_class_name_)
  );

  klass->members.for_each([this](const ClassMemberConstant &c) {
    if (c.local_name() == "class") {
      return;
    }

    // TODO сделать красиво
    kphp_error(c.local_name() == runtime_options_name_,
               fmt_format("Got unexpected {} constant '{}'",
                          configuration_class_name_, c.local_name()));

    auto arr = c.value.try_as<op_array>();
    kphp_error(arr, fmt_format("{}::{} must be a constexpr array",
                               configuration_class_name_, runtime_options_name_));
    for (const auto &opt : arr->args()) {
      auto opt_pair = opt.try_as<op_double_arrow>();
      kphp_error(opt_pair, fmt_format("{}::{} must be an associative map",
                                      configuration_class_name_, runtime_options_name_));
      const auto *opt_key = GenTree::get_constexpr_string(opt_pair->key());
      kphp_error_return(opt_key, fmt_format("{}::{} map keys must be constexpr strings",
                                            configuration_class_name_, runtime_options_name_));
      if (*opt_key == confdata_blacklist_key_) {
        register_confdata_blacklist(opt_pair->value());
      } else if (*opt_key == confdata_predefined_wildcard_) {
        register_confdata_predefined_wildcard(opt_pair->value());
      } else {
        kphp_error(0, fmt_format("Got unexpected option {}::{}['{}']",
                                 configuration_class_name_, runtime_options_name_, *opt_key));
      }
    }
  });
}

void RegisterKphpConfiguration::register_confdata_blacklist(VertexPtr value) const noexcept {
  const auto *opt_value = GenTree::get_constexpr_string(value);
  kphp_error_return(opt_value, fmt_format("{}::{} map values must be constexpr strings",
                                          configuration_class_name_, runtime_options_name_));

  re2::RE2 compiled_pattern{*opt_value, re2::RE2::CannedOptions::Quiet};
  kphp_error_return(compiled_pattern.ok(), fmt_format("Got bad regex pattern in {}::{}['{}']: {}",
                                                      configuration_class_name_, runtime_options_name_,
                                                      confdata_blacklist_key_, compiled_pattern.error()));
  G->add_kphp_runtime_opt(static_cast<std::string>(confdata_blacklist_key_));
  G->add_kphp_runtime_opt(*opt_value);
}

void RegisterKphpConfiguration::register_confdata_predefined_wildcard(VertexPtr value) const noexcept {
  auto wildcards = value.try_as<op_array>();
  kphp_error_return(wildcards, fmt_format("{}::{}['{}'] must be a constexpr array",
                                          configuration_class_name_, runtime_options_name_,
                                          confdata_predefined_wildcard_));
  for (const auto &wildcard : wildcards->args()) {
    const auto *wildcard_str_value = GenTree::get_constexpr_string(wildcard);
    kphp_error_return(wildcard_str_value && !wildcard_str_value->empty(),
                      fmt_format("{}::{}['{}'][] must be non empty constexpr string",
                                 configuration_class_name_, runtime_options_name_,
                                 confdata_predefined_wildcard_));
    G->add_kphp_runtime_opt(static_cast<std::string>(confdata_predefined_wildcard_));
    G->add_kphp_runtime_opt(*wildcard_str_value);
  }
}