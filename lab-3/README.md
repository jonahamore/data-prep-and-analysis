# Лабораторна робота №3 - Візуалізація даних

## Опис завдання

Виконати візуалізацію реального датасету з використанням Python:

- Обрати датасет типу **Multivariate** з атрибутами Categorical / Integer / Real та наявними пропущеними значеннями
- Провести **Data Cleaning**
- Побудувати **5–8 графіків**, що розкривають корисну інформацію про датасет

## Датасет

**Auto MPG** - характеристики автомобілів (398 записів, 9 атрибутів)  
Джерело: [UCI ML Repository - Auto MPG](https://archive.ics.uci.edu/dataset/9/auto+mpg)

Датасет завантажується автоматично при першому запуску ноутбука з UCI ML Repository.

## Структура файлів

```
lab-3/
├── lab3.ipynb         # Jupyter Notebook з аналізом та графіками
├── requirements.txt   # залежності Python
└── README.md          # цей файл
```

## Інструкція із запуску

### Вимоги

- Python **3.9+**
- pip

### Створення та активація віртуального середовища

**Windows:**
```bash
python -m venv venv
venv\Scripts\activate
```

**macOS / Linux:**
```bash
python3 -m venv venv
source venv/bin/activate
```

### Встановлення залежностей

```bash
pip install -r requirements.txt
```

### Запуск

```bash
jupyter notebook lab3.ipynb
```