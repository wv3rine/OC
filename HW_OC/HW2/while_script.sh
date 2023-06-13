#!/bin/bash
echo "Введите число, чтобы вывести все его делители"
read num
counter=1

# Производим суммирование от 0 до upperbound
# пока counter <= upper_bound
while [ $counter -le $num ]; do
  is_divisor=$(($num % $counter))
  if [[ $is_divisor -eq 0 ]]; then
    echo "Найден делитель $counter"
  fi
  # Увеличиваем переменную цикла
  counter=$(($counter + 1))
done