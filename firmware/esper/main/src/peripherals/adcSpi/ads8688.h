
enum COMMAND
{
    NO_OP = 0x0000,    // Continue operation in previous mode
    STDBY = 0x8200,    // Device is placed into standby mode
    PWR_DN = 0x8300,   // Device is powered down
    RST = 0x8500,      // Program register is reset to default
    AUTO_RST = 0xA000, // Auto mode enabled following a reset
    MAN_Ch_0 = 0xC000, // Channel 0 input is selected
    MAN_Ch_1 = 0xC400, // Channel 1 input
    MAN_Ch_2 = 0xC800, // Channel 2 input
    MAN_Ch_3 = 0xCC00, // Channel 3 input
    MAN_Ch_4 = 0xD000, // Channel 4 input
    MAN_Ch_5 = 0xD400, // Channel 5 input
    MAN_Ch_6 = 0xD800, // Channel 6 input
    MAN_Ch_7 = 0xDC00, // Channel 7 input
    MAN_AUX = 0xE000,  // AUX channel input is selected
};

enum registers
{
    AUTO_SEQ_EN = 0x01, // Auto Sequencing Enable: default 0xFF - bitX to enable chX
    CH_PWR_DN = 0x02,   // Channel Power Down: default 0x00 - bitX to power down chX
    FEAT_SEL = 0x03,    // Feature Select: default 0x00
    RANGE_0 = 0x05,     // Range Select for Channel 0 - 0b0000 +-2.5xVref | 0b0001 +-1.25xVref | 0b0010 +-0.625xVref | 0b0101 +2.5xVref | 0b110 1.25xVref
    RANGE_1 = 0x06,     // Range Select for Channel 1
    RANGE_2 = 0x07,     // Range Select for Channel 2
    RANGE_3 = 0x08,     // Range Select for Channel 3
    RANGE_4 = 0x09,     // Range Select for Channel 4
    RANGE_5 = 0x0A,     // Range Select for Channel 5
    RANGE_6 = 0x0B,     // Range Select for Channel 6
    RANGE_7 = 0x0C,     // Range Select for Channel 7
    READBACK = 0x3F,    // Allows to read the mode of operation of the device
};

enum read_write_flags
{
    CMD_READ_REG = 0x00, // Read register command flag
    CMD_WRITE_REG = 0x01 // Write register command flag
};