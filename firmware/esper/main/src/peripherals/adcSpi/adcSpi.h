#include "update.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void configAdcSpi();

    void get_full_conversion_polling(void);
    void handleProbe();

    extern uint16_t inVal[4];
    extern float inValF[4];

    void configAdcEnabled(struct enableAdc enable);

#ifdef __cplusplus
}
#endif
