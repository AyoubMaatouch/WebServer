<?php

    session_start();
    $_SESSION["test"] = "tets";
    $value = 'value_test';

    setcookie("cookie_test", $value);

    echo "hello";
?>