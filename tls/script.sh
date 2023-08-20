#!/bin/bash

# Разделение исходного файла на 20 частей
split -l 10 domains.txt temp_file_

# Запуск программы для каждого файла
for i in temp_file_*; do
     ./my_program "$i" "${i}.output" &
done

# Ожидание завершения всех запущенных программ
wait

# Объединение результатов в один файл
cat temp_file_*.output > combined_output.txt

# Удаление временных файлов
rm temp_file_*
rm temp_file_*.output

echo "Все операции завершены."
