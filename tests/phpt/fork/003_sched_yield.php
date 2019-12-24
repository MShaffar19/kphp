@ok
<?php
require_once __DIR__."/../dl/polyfill/fork-php-polyfill.php";

$fork_calls = 1000;
$func_iterations = 10;

#ifndef KittenPHP
for ($i = 0; $i <= $func_iterations; $i++) {
  for ($id = 0; $id <= $fork_calls; $id++) {
    echo "f $id $i\n";
  }
}
return;
#endif

$gid = 0;

function f() {
  global $func_iterations;
  global $gid;
  $id = $gid++;
  for ($i = 0; $i <= $func_iterations; $i++) {
    sched_yield();
    echo "f $id $i\n";
  }
}

$last = false;

for ($i = 0; $i <= $fork_calls; $i++) {
  $last = fork(f());
}

wait($last);