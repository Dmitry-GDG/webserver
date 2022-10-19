<?php
if (isset($_FILES['avatar'])) {
    $file = $_FILES['avatar'];

    print("Загружен файл с именем " . $file['name'] . " и размером " . $file['size'] . " байт");
}