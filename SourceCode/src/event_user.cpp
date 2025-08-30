#include <event_user.h>

// 新增：前置声明
static void user_chart_refresh(void);

// 新增：图表序列静态指针
static lv_chart_series_t *s_user_chart_ser = nullptr;

// 新增：统一管理X轴点数（改为350）
static const uint16_t kChartPointCount = 350;

void ui_event_UserSettingBack(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_RELEASED)
    {
        _ui_screen_change(&ui_SystemSettingScreen, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_SystemSettingScreen_screen_init);
        lvgl_group_to_setting();
    }
}

// 在每个温度/时间事件中，更新变量后追加调用 user_chart_refresh()

void ui_event_Temp1(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        // 新增：先确保滑块最大值受 HeaterTargetTempMax 限制
        lv_slider_set_range(target, 0, HeaterTargetTempMax);
        _ui_slider_set_text_value(ui_TextTemp1, target, "", "℃");
        Temp1 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增：实时刷新曲线
    }
}

void ui_event_Temp2(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        lv_slider_set_range(target, 0, HeaterTargetTempMax); // 新增
        _ui_slider_set_text_value(ui_TextTemp2, target, "", "℃");
        Temp2 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

void ui_event_Temp3(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        lv_slider_set_range(target, 0, HeaterTargetTempMax); // 新增
        _ui_slider_set_text_value(ui_TextTemp3, target, "", "℃");
        Temp3 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

void ui_event_Temp4(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        lv_slider_set_range(target, 0, HeaterTargetTempMax); // 新增
        _ui_slider_set_text_value(ui_TextTemp4, target, "", "℃");
        Temp4 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

void ui_event_Temp5(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        lv_slider_set_range(target, 0, HeaterTargetTempMax); // 新增
        _ui_slider_set_text_value(ui_TextTemp5, target, "", "℃");
        Temp5 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

void ui_event_Time1(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        _ui_slider_set_text_value(ui_TextTime1, target, "", "S");
        Time1 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

void ui_event_Time2(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        _ui_slider_set_text_value(ui_TextTime2, target, "", "S");
        Time2 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

void ui_event_Time3(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        _ui_slider_set_text_value(ui_TextTime3, target, "", "S");
        Time3 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

void ui_event_Time4(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        _ui_slider_set_text_value(ui_TextTime4, target, "", "S");
        Time4 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

void ui_event_Time5(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        _ui_slider_set_text_value(ui_TextTime5, target, "", "S");
        Time5 = lv_slider_get_value(target);
        user_chart_refresh(); // 新增
    }
}

// 新增：图表初始化（上电/界面创建后调用一次）
void user_chart_init(void)
{
    if (!ui_UserChart) return;

    // 基本配置（点数改为350）
    lv_chart_set_type(ui_UserChart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(ui_UserChart, kChartPointCount);
    // 自适应Y轴范围由 user_chart_refresh 设置

    // 创建或复用序列
    if (!s_user_chart_ser) {
#if LVGL_VERSION_MAJOR >= 8
        s_user_chart_ser = lv_chart_add_series(ui_UserChart, lv_color_hex(0xE53935), LV_CHART_AXIS_PRIMARY_Y);
#else
        s_user_chart_ser = lv_chart_add_series(ui_UserChart, LV_COLOR_RED);
#endif
    }

    // 先填充为50℃
    for (uint16_t i = 0; i < kChartPointCount; ++i) {
#if LVGL_VERSION_MAJOR >= 8
        lv_chart_set_value_by_id(ui_UserChart, s_user_chart_ser, i, 50);
#else
        s_user_chart_ser->points[i] = 50;
#endif
    }

    // 新增：上电时设置Temp1~5滑动条最大值并修正当前值
    user_temp_sliders_refresh_max(); // 内部会刷新曲线
}

// 新增：按Temp1~5与Time1~5生成曲线（起点/终点固定为50℃，Time为横坐标值，非叠加）
static void user_chart_refresh(void)
{
    if (!ui_UserChart) return;

    const int kMaxX = kChartPointCount - 1;

    // 确保序列存在
    if (!s_user_chart_ser) {
#if LVGL_VERSION_MAJOR >= 8
        s_user_chart_ser = lv_chart_add_series(ui_UserChart, lv_color_hex(0xE53935), LV_CHART_AXIS_PRIMARY_Y);
#else
        s_user_chart_ser = lv_chart_add_series(ui_UserChart, LV_COLOR_RED);
#endif
        if (!s_user_chart_ser) return;
    }

    auto clampi = [](int v, int lo, int hi) {
        if (v < lo) return lo;
        if (v > hi) return hi;
        return v;
    };

    // 自适应Y轴：基于5个温度的最大值，但至少包含50℃（起/终点为50）
    int temps_raw[5] = { Temp1, Temp2, Temp3, Temp4, Temp5 };
    int yMax = 50;
    for (int i = 0; i < 5; ++i) if (temps_raw[i] > yMax) yMax = temps_raw[i];
    if (yMax < 1) yMax = 1; // 安全保护，避免上下限相等

    lv_chart_set_range(ui_UserChart, LV_CHART_AXIS_PRIMARY_Y, 0, yMax);

    // 构造(Time, Temp)点，Time为横坐标（不叠加），并按Time升序排序
    struct P { int x; int y; };
    P pts[5] = {
        { clampi(Time1, 0, kMaxX), temps_raw[0] },
        { clampi(Time2, 0, kMaxX), temps_raw[1] },
        { clampi(Time3, 0, kMaxX), temps_raw[2] },
        { clampi(Time4, 0, kMaxX), temps_raw[3] },
        { clampi(Time5, 0, kMaxX), temps_raw[4] },
    };
    // 简单插入排序
    for (int i = 1; i < 5; ++i) {
        P key = pts[i];
        int j = i - 1;
        while (j >= 0 && pts[j].x > key.x) { pts[j + 1] = pts[j]; --j; }
        pts[j + 1] = key;
    }

    // 关键点序列：P0(0,50) -> P1..P5(按Time排序后) -> Pend(kMaxX,50)
    int kx[7];
    int ky[7];
    int kcnt = 0;
    kx[kcnt] = 0;     ky[kcnt] = 50; kcnt++;

    for (int i = 0; i < 5; ++i) {
        kx[kcnt] = pts[i].x;
        ky[kcnt] = clampi(pts[i].y, 0, yMax);
        kcnt++;
    }

    if (kx[kcnt - 1] != kMaxX) {
        kx[kcnt] = kMaxX;
        ky[kcnt] = 50;
        kcnt++;
    } else {
        // 若最后一个关键点已在kMaxX，覆盖为50以满足“结束为50℃”
        ky[kcnt - 1] = 50;
    }

    // 先整体置为50℃
    for (int i = 0; i <= kMaxX; ++i) {
#if LVGL_VERSION_MAJOR >= 8
        lv_chart_set_value_by_id(ui_UserChart, s_user_chart_ser, i, 50);
#else
        s_user_chart_ser->points[i] = 50;
#endif
    }

    // 线性插值每一段（允许dx=0的点，仅赋值该点）
    for (int s = 0; s < kcnt - 1; ++s) {
        int x0 = kx[s], y0 = ky[s];
        int x1 = kx[s + 1], y1 = ky[s + 1];
        int dx = x1 - x0;

        if (dx <= 0) {
#if LVGL_VERSION_MAJOR >= 8
            lv_chart_set_value_by_id(ui_UserChart, s_user_chart_ser, clampi(x1, 0, kMaxX), clampi(y1, 0, yMax));
#else
            s_user_chart_ser->points[clampi(x1, 0, kMaxX)] = clampi(y1, 0, yMax);
#endif
            continue;
        }

        for (int x = x0; x <= x1; ++x) {
            // y = y0 + (y1-y0)*(x-x0)/dx （四舍五入）
            int num = (y1 - y0) * (x - x0);
            int y = y0 + (num >= 0 ? (num + dx / 2) / dx : (num - dx / 2) / dx);
            y = clampi(y, 0, yMax);
#if LVGL_VERSION_MAJOR >= 8
            lv_chart_set_value_by_id(ui_UserChart, s_user_chart_ser, x, y);
#else
            s_user_chart_ser->points[x] = y;
#endif
        }
    }

    lv_chart_refresh(ui_UserChart);
}

// 新增：根据 HeaterTargetTempMax 重新设置 Temp1~5 的最大值并校正显示与曲线
void user_temp_sliders_refresh_max(void)
{
    int maxv = HeaterTargetTempMax;
    if (maxv < 0) maxv = 0;

    // 修复：第三个参数改为 uint16_t&，并用局部int做夹取后回写
    auto apply_one = [&](lv_obj_t* slider, lv_obj_t* label, uint16_t& tempVar) {
        if (!slider) return;
        lv_slider_set_range(slider, 0, maxv);
        int v = (int)tempVar;
        if (v > maxv) v = maxv;
        if (v < 0) v = 0;
        tempVar = (uint16_t)v;
        lv_slider_set_value(slider, tempVar, LV_ANIM_OFF);
        if (label) _ui_slider_set_text_value(label, slider, "", "℃");
    };

    apply_one(ui_Temp1, ui_TextTemp1, Temp1);
    apply_one(ui_Temp2, ui_TextTemp2, Temp2);
    apply_one(ui_Temp3, ui_TextTemp3, Temp3);
    apply_one(ui_Temp4, ui_TextTemp4, Temp4);
    apply_one(ui_Temp5, ui_TextTemp5, Temp5);

    // 重新绘制曲线
    user_chart_refresh();
}
