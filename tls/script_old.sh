#!/bin/bash

# Разделение исходного файла на 20 частей
split -l 30000 input.txt temp_file_

# Функция для запуска программы и сохранения вывода
run_program() {
    input_file="$1"
    output_file="${input_file}.output"
    ./my_program "$input_file" > "$output_file"
}

export -f run_program

# Запуск программ с использованием GNU Parallel
ls temp_file_* | parallel run_program

# Объединение результатов в один файл
cat temp_file_*.output > combined_output.txt

# Удаление временных файлов
rm temp_file_*
rm temp_file_*.output

echo "Все операции завершены."
