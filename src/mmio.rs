pub const MMIO_BASE: u32 = 0x3F00_0000;

#[repr(u32)]
#[derive(Copy, Clone)]
pub enum AuxReg {
    Enable = 0x0021_5000,
    MuIo = 0x0021_5040,
    MuIer = 0x0021_5044,
    MuIir = 0x0021_5048,
    MuLcr = 0x0021_504C,
    MuMcr = 0x0021_5050,
    MuLsr = 0x0021_5054,
    MuMsr = 0x0021_5058,
    MuScratch = 0x0021_505C,
    MuCntl = 0x0021_5060,
    MuStat = 0x0021_5064,
    MuBaud = 0x0021_5068,
}

#[repr(u32)]
#[derive(Copy, Clone)]
pub enum GpioReg {
    Gpfsel0 = 0x0020_0000,
    Gpfsel1 = 0x0020_0004,
    Gpfsel2 = 0x0020_0008,
    Gpfsel3 = 0x0020_000C,
    Gpfsel4 = 0x0020_0010,
    Gpfsel5 = 0x0020_0014,
    Gpset0 = 0x0020_001C,
    Gpset1 = 0x0020_0020,
    Gpclr0 = 0x0020_0028,
    Gpclr1 = 0x0020_002C,
    Gplev0 = 0x0020_0034,
    Gplev1 = 0x0020_0038,
    Gpeds0 = 0x0020_0040,
    Gpeds1 = 0x0020_0044,
    Gphen0 = 0x0020_004C,
    Gphen1 = 0x0020_0050,
    Gplen0 = 0x0020_0058,
    Gplen1 = 0x0020_005C,
    Gpren0 = 0x0020_0064,
    Gpren1 = 0x0020_0068,
    Gpfen0 = 0x0020_0070,
    Gpfen1 = 0x0020_0074,
    Gphs0 = 0x0020_007C,
    Gphs1 = 0x0020_0080,
    Gppud = 0x0020_0094,
    GppudClk0 = 0x0020_0098,
    GppudClk1 = 0x0020_009C,
}

#[repr(u32)]
#[derive(Copy, Clone)]
pub enum PmReg {
    Password = 0x0010_0000,
    Wdog = 0x0010_1000,
    Rstc = 0x0010_1004,
    Rsts = 0x0010_1008,
}

#[derive(Copy, Clone)]
pub enum MmioReg {
    Aux(AuxReg),
    Gpio(GpioReg),
    Pm(PmReg),
}

impl MmioReg {
    pub fn addr(&self) -> u32 {
        match self {
            MmioReg::Aux(reg) => MMIO_BASE + *reg as u32,
            MmioReg::Gpio(reg) => MMIO_BASE + *reg as u32,
            MmioReg::Pm(reg) => MMIO_BASE + *reg as u32,
        }
    }
}

pub struct MMIO;

impl MMIO {
    pub unsafe fn write_reg(reg: MmioReg, value: u32) {
        let addr = reg.addr();
        core::ptr::write_volatile(addr as *mut u32, value);
    }

    pub unsafe fn read_reg(reg: MmioReg) -> u32 {
        let addr = reg.addr();
        core::ptr::read_volatile(addr as *const u32)
    }

    pub fn delay(count: u32) {
        for _ in 0..count {
            unsafe {
                core::arch::asm!("nop");
            }
        }
    }

    pub unsafe fn reboot() {
        let mut rsts = MMIO::read_reg(MmioReg::Pm(PmReg::Rsts));
        rsts &= !(0b111 << 20);
        rsts |= 0x5A000;
        MMIO::write_reg(MmioReg::Pm(PmReg::Rsts), rsts);
        MMIO::delay(150);
        MMIO::write_reg(MmioReg::Pm(PmReg::Wdog), 0x30_0000);
    }
}
