# Лабораторна робота №2 — Аналіз даних з Pandas

**Студент:** Лебідь Антон  
**Група:** ФБ-46  
**Дисципліна:** Засоби підготовки та аналізу даних

---

## Опис

Два Jupyter-ноутбуки з аналізом реальних датасетів засобами бібліотеки **Pandas**.

---

## Частина 1 — Аналіз індексу здоров'я рослинності (VHI)

**Файл:** `part1_vhi.ipynb`

Дані: NOAA VHI (Vegetation Health Index) по 27 областях України, 1981–2024 рр.

Реалізовано:
- Завантаження CSV-файлів з API NOAA для кожної з 27 областей
- Маппінг NOAA-індексів на офіційні UA-коди областей
- Очищення та об'єднання даних у єдиний DataFrame
- Функції фільтрації VHI за областю, роком, діапазоном років
- Статистика (min / max / mean / median) по обраних областях

---

## Частина 2 — Аналіз споживання електроенергії

**Файл:** `part2_power.ipynb`

Дані: [Individual household electric power consumption](https://archive.ics.uci.edu/ml/datasets/individual+household+electric+power+consumption) — хвилинні вимірювання одного домогосподарства (Франція, 2006–2010).

Реалізовано:
- Завантаження та очищення даних (заповнення NaN медіаною)
- Фільтрації: потужність > 5 кВт, сила струму 19–20 А, вечірнє пікове споживання
- Замір часу виконання через `timeit`
- Нормалізація (Min-Max) та стандартизація (Z-score)
- Кореляційний аналіз (Пірсон / Спірмен)
- One Hot Encoding атрибуту «частина доби»

---

## Вимоги до системи

- **Python:** 3.10+
- **ОС:** Windows / macOS / Linux

## Встановлення залежностей

```bash
python -m venv venv
venv\Scripts\activate      # Windows
# або
source venv/bin/activate   # macOS / Linux

pip install -r requirements.txt
```

## Запуск

```bash
jupyter lab
```

Відкрити `part1_vhi.ipynb` або `part2_power.ipynb` у браузері.

> **Примітка:** файл `data/household_power_consumption.txt` (~500 MB) не включено до репозиторію.  
> Завантажити вручну: [UCI ML Repository](https://archive.ics.uci.edu/ml/datasets/individual+household+electric+power+consumption) → розпакувати в папку `data/`.
