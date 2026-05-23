import io
import os
import urllib.request

import matplotlib.pyplot as plt
import pandas as pd
import streamlit as st
from matplotlib.lines import Line2D

# Константи

REGION_NAMES = {
    1:  "Черкаська",
    2:  "Чернігівська",
    3:  "Чернівецька",
    4:  "Кримська",
    5:  "Дніпропетровська",
    6:  "Донецька",
    7:  "Івано-Франківська",
    8:  "Харківська",
    9:  "Херсонська",
    10: "Хмельницька",
    11: "Київська",
    12: "Київ (місто)",
    13: "Кіровоградська",
    14: "Луганська",
    15: "Львівська",
    16: "Миколаївська",
    17: "Одеська",
    18: "Полтавська",
    19: "Рівненська",
    20: "Сумська",
    21: "Тернопільська",
    22: "Вінницька",
    23: "Волинська",
    24: "Закарпатська",
    25: "Запорізька",
    26: "Житомирська",
    27: "Севастополь",
}

METRICS = ["VCI", "TCI", "VHI"]
DATA_DIR = os.path.join(os.path.dirname(__file__), "vhi_data")
DEFAULT_METRIC = "VHI"
DEFAULT_REGION = "Київська"
YEAR_MIN_GLOBAL = 1981
YEAR_MAX_GLOBAL = 2024


# Завантаження даних

def _parse_vhi_csv(content: str, region_id: int) -> pd.DataFrame:
    # Прибираємо HTML-рядки та порожні рядки
    lines = [
        line.strip()
        for line in content.split("\n")
        if line.strip() and not line.strip().startswith("<")
    ]
    csv_text = "\n".join(lines)
    # NOAA додає зайву кому після колонки vhi, тому читаємо 8 колонок і видаляємо зайву
    df = pd.read_csv(
        io.StringIO(csv_text),
        names=["year", "week", "smn", "smt", "vci", "tci", "vhi", "extra"],
        skipinitialspace=True,
        on_bad_lines="skip",
    )
    df = df.drop(columns=["extra"], errors="ignore")
    # Видаляємо нечислові рядки заголовків, які могли потрапити у дані
    df = df[pd.to_numeric(df["year"], errors="coerce").notna()].copy()
    for col in ["year", "week"]:
        df[col] = df[col].astype(int)
    for col in ["vci", "tci", "vhi"]:
        df[col] = pd.to_numeric(df[col], errors="coerce")
    df = df.dropna(subset=["vci", "tci", "vhi"])
    df["region_id"] = region_id
    df["region"] = REGION_NAMES[region_id]
    return df


def _download_region(region_id: int) -> pd.DataFrame:
    url = (
        "https://www.star.nesdis.noaa.gov/smcd/emb/vci/VH/get_TS_admin.php"
        f"?country=UKR&provinceID={region_id}&year1=1981&year2=2024&type=Mean"
    )
    with urllib.request.urlopen(url, timeout=30) as resp:
        content = resp.read().decode("utf-8")
    return _parse_vhi_csv(content, region_id)


@st.cache_data(show_spinner="Завантаження даних VHI...")
def load_all_data() -> pd.DataFrame:
    # Завантажуємо дані для кожної області, кешуємо локально
    os.makedirs(DATA_DIR, exist_ok=True)
    frames = []
    failed = []

    for region_id, region_name in REGION_NAMES.items():
        cache_path = os.path.join(DATA_DIR, f"vhi_{region_id}.csv")
        if os.path.exists(cache_path):
            frames.append(pd.read_csv(cache_path))
        else:
            try:
                df = _download_region(region_id)
                df.to_csv(cache_path, index=False)
                frames.append(df)
            except Exception as exc:
                failed.append(f"{region_name} (id={region_id}): {exc}")

    if failed:
        st.warning("Не вдалось завантажити:\n" + "\n".join(failed))

    if not frames:
        st.error("Дані відсутні. Перевірте підключення до інтернету.")
        st.stop()

    return pd.concat(frames, ignore_index=True)


# Стан сесії

def _init_state(year_min: int, year_max: int) -> None:
    # Встановлюємо початкові значення фільтрів при першому завантаженні
    defaults = {
        "metric":     DEFAULT_METRIC,
        "region":     DEFAULT_REGION,
        "week_range": (1, 52),
        "year_range": (year_min, year_max),
        "sort_asc":   False,
        "sort_desc":  False,
    }
    for key, val in defaults.items():
        if key not in st.session_state:
            st.session_state[key] = val


def _reset(year_min: int, year_max: int) -> None:
    # Скидаємо всі фільтри до початкових значень
    st.session_state["metric"]     = DEFAULT_METRIC
    st.session_state["region"]     = DEFAULT_REGION
    st.session_state["week_range"] = (1, 52)
    st.session_state["year_range"] = (year_min, year_max)
    st.session_state["sort_asc"]   = False
    st.session_state["sort_desc"]  = False


# Побудова графіків

def _chart_timeseries(df: pd.DataFrame, metric: str, region: str,
                       week_range: tuple, year_range: tuple) -> plt.Figure:
    # Лінійний графік обраного показника для вибраної області
    plot_df = df.sort_values(["year", "week"]).reset_index(drop=True)
    metric_col = metric.lower()

    fig, ax = plt.subplots(figsize=(12, 4))
    ax.plot(plot_df.index, plot_df[metric_col],
            color="#2563EB", linewidth=1.4, label=metric)
    ax.fill_between(plot_df.index, plot_df[metric_col],
                    alpha=0.15, color="#2563EB")

    # Мітки на осі X - перший тиждень кожного року
    year_ticks, year_labels, prev = [], [], None
    for i, row in plot_df.iterrows():
        if row["year"] != prev:
            year_ticks.append(i)
            year_labels.append(str(int(row["year"])))
            prev = row["year"]

    step = max(1, len(year_ticks) // 15)
    ax.set_xticks(year_ticks[::step])
    ax.set_xticklabels(year_labels[::step], rotation=45, ha="right")

    ax.set_title(
        f"{metric} - {region}  "
        f"({year_range[0]}-{year_range[1]},  тижні {week_range[0]}-{week_range[1]})",
        fontsize=13,
    )
    ax.set_xlabel("Рік")
    ax.set_ylabel(metric)
    ax.grid(True, alpha=0.25)
    ax.legend()
    plt.tight_layout()
    return fig


def _chart_comparison(all_df: pd.DataFrame, filtered_df: pd.DataFrame,
                       metric: str, region: str,
                       week_range: tuple, year_range: tuple) -> plt.Figure:
    # Порівняння середніх значень показника по всіх областях
    metric_col = metric.lower()

    region_yearly = (
        all_df.groupby(["region", "year"])[metric_col]
        .mean()
        .reset_index()
    )

    fig, ax = plt.subplots(figsize=(12, 5))

    # Інші області сірим кольором
    for rgn, grp in region_yearly.groupby("region"):
        if rgn != region:
            ax.plot(grp["year"], grp[metric_col],
                    color="#CBD5E1", linewidth=0.9, alpha=0.8)

    # Обрана область червоним кольором
    sel = region_yearly[region_yearly["region"] == region]
    ax.plot(sel["year"], sel[metric_col],
            color="#DC2626", linewidth=2.5, zorder=5)

    legend_handles = [
        Line2D([0], [0], color="#DC2626", linewidth=2.5, label=region),
        Line2D([0], [0], color="#CBD5E1", linewidth=1.5, label="Інші області"),
    ]
    ax.legend(handles=legend_handles, loc="upper right")
    ax.set_title(
        f"Порівняння {metric} по областях  "
        f"({year_range[0]}-{year_range[1]},  тижні {week_range[0]}-{week_range[1]})",
        fontsize=13,
    )
    ax.set_xlabel("Рік")
    ax.set_ylabel(f"Середній {metric}")
    ax.grid(True, alpha=0.25)
    plt.tight_layout()
    return fig


# Головна функція

def main() -> None:
    st.set_page_config(
        page_title="VHI Аналіз України",
        page_icon="🌿",
        layout="wide",
    )
    st.title("Аналіз індексів вегетаційного здоров'я (VHI/TCI/VCI) - Україна")

    df = load_all_data()
    year_min = int(df["year"].min())
    year_max = int(df["year"].max())
    region_list = sorted(df["region"].unique())

    _init_state(year_min, year_max)

    # Розбиваємо сторінку на дві колонки: ліва - фільтри, права - результати
    col_ctrl, col_main = st.columns([1, 3], gap="large")

    with col_ctrl:
        st.subheader("Параметри")

        metric = st.selectbox(
            "Показник (часовий ряд)",
            METRICS,
            index=METRICS.index(st.session_state["metric"]),
            key="metric",
        )

        region_idx = (
            region_list.index(st.session_state["region"])
            if st.session_state["region"] in region_list
            else 0
        )
        region = st.selectbox(
            "Область",
            region_list,
            index=region_idx,
            key="region",
        )

        week_range = st.slider(
            "Інтервал тижнів",
            min_value=1,
            max_value=52,
            value=st.session_state["week_range"],
            key="week_range",
        )

        year_range = st.slider(
            "Інтервал років",
            min_value=year_min,
            max_value=year_max,
            value=st.session_state["year_range"],
            key="year_range",
        )

        st.markdown("**Сортування таблиці**")
        sort_asc  = st.checkbox("За зростанням",  value=st.session_state["sort_asc"],  key="sort_asc")
        sort_desc = st.checkbox("За спаданням",   value=st.session_state["sort_desc"], key="sort_desc")
        if sort_asc and sort_desc:
            st.warning("Обидва чекбокси активні - пріоритет за зростанням.")

        st.button(
            "Скинути фільтри",
            on_click=_reset,
            args=(year_min, year_max),
            use_container_width=True,
        )

    # Фільтрація даних
    metric_col = metric.lower()

    filtered = df[
        (df["region"] == region)
        & df["week"].between(week_range[0], week_range[1])
        & df["year"].between(year_range[0], year_range[1])
    ].copy()

    all_filtered = df[
        df["week"].between(week_range[0], week_range[1])
        & df["year"].between(year_range[0], year_range[1])
    ].copy()

    # Сортування
    if sort_asc:
        filtered = filtered.sort_values(metric_col, ascending=True)
    elif sort_desc:
        filtered = filtered.sort_values(metric_col, ascending=False)
    else:
        filtered = filtered.sort_values(["year", "week"])

    # Вкладки з результатами
    with col_main:
        tab_table, tab_chart, tab_compare = st.tabs(
            ["Таблиця", "Часовий ряд", "Порівняння областей"]
        )

        with tab_table:
            display_cols = ["region", "year", "week", "vci", "tci", "vhi"]
            st.dataframe(
                filtered[display_cols].reset_index(drop=True),
                use_container_width=True,
                height=520,
            )
            st.caption(f"Записів відображено: **{len(filtered):,}**")

        with tab_chart:
            if filtered.empty:
                st.info("Немає даних для відображення. Змініть параметри фільтрації.")
            else:
                fig = _chart_timeseries(
                    filtered, metric, region, week_range, year_range
                )
                st.pyplot(fig)
                plt.close(fig)

        with tab_compare:
            if all_filtered.empty:
                st.info("Немає даних для відображення. Змініть параметри фільтрації.")
            else:
                fig2 = _chart_comparison(
                    all_filtered, filtered, metric, region, week_range, year_range
                )
                st.pyplot(fig2)
                plt.close(fig2)


if __name__ == "__main__":
    main()
