#include "st7789.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>

int spi_fd;

struct gpiod_chip *chip = NULL;
struct gpiod_line_settings *settings = NULL;
struct gpiod_line_config *config = NULL;
struct gpiod_request_config *ReqConf = NULL;
struct gpiod_line_request *LineReq = NULL;

// Inicializar SPI
int spi_init(void) {
    spi_fd = open(SPI_DEV_PATH, O_RDWR);
    if (spi_fd < 0) {
        perror("ERROR AL ABRIR SPI");
        return -1;
    }

    uint8_t mode = SPI_MODE_3;
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0) {
        perror("ERROR AL CONFIGURAR MODO SPI");
        close(spi_fd);
        return -1;
    }

    uint8_t bits = 8;
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        perror("ERROR AL CONFIGURAR NUMERO DE BITS");
        close(spi_fd);
        return -1;
    }

    uint32_t speed = 1000000;  // 1 MHz
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        perror("ERROR AL CONFIGURAR VELOCIDAD SPI");
        close(spi_fd);
        return -1;
    }

    return 0;
}

// Inicializar GPIO
int gpio_init(void) {
    chip = gpiod_chip_open(CHIP);
    if (!chip) {
        perror("ERROR AL ABRIR CHIP");
        return -1;
    }

    settings = gpiod_line_settings_new();
    if (!settings) {
        perror("ERROR AL ABRIR AJUSTES");
        gpiod_chip_close(chip);
        return -1;
    }

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);

    unsigned int offsets[] = {RESET, DC, BLK};

    config = gpiod_line_config_new();
    if (!config) {
        perror("ERROR AL ABRIR CONFIGURACION");
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        return -1;
    }

    if (gpiod_line_config_add_line_settings(config, offsets, 3, settings) < 0) {
        perror("ERROR AL AGREGAR CONFIG A SETTINGS");
        gpiod_line_config_free(config);
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        return -1;
    }

    ReqConf = gpiod_request_config_new();
    if (!ReqConf) {
        perror("ERROR AL PEDIR CONFIGURACION");
        gpiod_line_config_free(config);
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        return -1;
    }

    gpiod_request_config_set_consumer(ReqConf, CONSUMER);

    LineReq = gpiod_chip_request_lines(chip, ReqConf, config);
    if (!LineReq) {
        perror("ERROR AL SOLICITAR LINEAS");
        gpiod_request_config_free(ReqConf);
        gpiod_line_config_free(config);
        gpiod_line_settings_free(settings);
        gpiod_chip_close(chip);
        return -1;
    }

    gpiod_line_request_set_value(LineReq, BLK, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(LineReq, DC, GPIOD_LINE_VALUE_INACTIVE);
    gpiod_line_request_set_value(LineReq, RESET, GPIOD_LINE_VALUE_ACTIVE);

    return 0;
}

// Cambiar DC: 0 = comando, 1 = dato
int dc_set(int is_data) {
    enum gpiod_line_value val = is_data ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
    if (gpiod_line_request_set_value(LineReq, DC, val) < 0) {
        perror("ERROR AL CAMBIAR DC");
        return -1;
    }
    return 0;
}

// Función para enviar datos a través de SPI
int spi_write_bytes(const uint8_t *data, size_t len) {
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)data,
        .rx_buf = 0,
        .len = len,
        .speed_hz = 10000000,   // 10 MHz
        .bits_per_word = 8,
        .delay_usecs = 0,
    };

    // Realizar la transferencia SPI
    int ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 0) {
        perror("Error en SPI_IOC_MESSAGE");
        return -1;
    }
    return 0;
}

// Función para enviar comandos al ST7789
int st7789_write_cmd(uint8_t cmd) {
    if (dc_set(0) < 0) return -1;
    return spi_write_bytes(&cmd, 1);
}

// Función para escribir datos
int st7789_write_data_bytes(const uint8_t *data, size_t len) {
    if (dc_set(1) < 0) return -1;
    return spi_write_bytes(data, len);
}



int st7789_write_data_byte(uint8_t data) {
    if (dc_set(1) < 0) return -1;
    return spi_write_bytes(&data, 1);
}



// Definir ventana de escritura en el ST7789
int st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint16_t xs = XSTART + x0;
    uint16_t xe = XSTART + x1;
    uint16_t ys = YSTART + y0;
    uint16_t ye = YSTART + y1;

    uint8_t buf[4];

    if (st7789_write_cmd(0x2A) < 0) return -1;
    buf[0] = (xs >> 8) & 0xFF;
    buf[1] = (xs & 0xFF);
    buf[2] = (xe >> 8) & 0xFF;
    buf[3] = (xe & 0xFF);
    if (st7789_write_data_bytes(buf, 4) < 0) return -1;

    if (st7789_write_cmd(0x2B) < 0) return -1;
    buf[0] = (ys >> 8) & 0xFF;
    buf[1] = (ys & 0xFF);
    buf[2] = (ye >> 8) & 0xFF;
    buf[3] = (ye & 0xFF);
    if (st7789_write_data_bytes(buf, 4) < 0) return -1;

    if (st7789_write_cmd(0x2C) < 0) return -1;

    return 0;
}


// Inicializar ST7789
int st7789_init(void) {
    if (st7789_write_cmd(0x01) < 0) return -1;
    usleep(150000);

    if (st7789_write_cmd(0x11) < 0) return -1;
    usleep(120000);

    if (st7789_write_cmd(0x3A) < 0) return -1;
    if (st7789_write_data_byte(0x55) < 0) return -1;
    usleep(10000);

    if (st7789_write_cmd(0x36) < 0) return -1;
    if (st7789_write_data_byte(0x60) < 0) return -1;

    if (st7789_write_cmd(0x21) < 0) return -1;
    usleep(10000);

    if (st7789_write_cmd(0x13) < 0) return -1;
    usleep(10000);

    if (st7789_write_cmd(0x29) < 0) return -1;
    usleep(10000);

    return 0;
}

// Llenar la pantalla con un color
int st7789_fill_color(uint16_t color) {
    st7789_set_window(0, 0, WIDTH - 1, HEIGHT - 1);
    uint8_t buf[256];
    for (int i = 0; i < sizeof(buf); i += 2) {
        buf[i] = (color >> 8) & 0xFF;
        buf[i + 1] = color & 0xFF;
    }

    size_t total_pixels = WIDTH * HEIGHT;
    size_t pixels_per_chunk = sizeof(buf) / 2;
    size_t sent = 0;

    while (sent < total_pixels) {
        size_t to_send = total_pixels - sent;
        if (to_send > pixels_per_chunk)
            to_send = pixels_per_chunk;

        if (st7789_write_data_bytes(buf, to_send * 2) < 0)
            return -1;

        sent += to_send;
    }

    return 0;
}