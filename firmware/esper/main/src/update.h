#pragma once

void configUpdate(void);

struct enableAdc
{
    bool in[4];
};

extern struct enableOut enable_out;

extern TaskHandle_t update_task_handle;
