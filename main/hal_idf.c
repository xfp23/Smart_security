#include "hal_idf.h"

static QueueHandle_t uart0_queue;
static QueueHandle_t uart1_queue;
static QueueHandle_t uart2_queue;

void create_new_uart(UART_DEV *dev)
{
    dev->init = HAL_init_uart;
    dev->read_byte = HAL_read_bytes;
    dev->send_byte = HAL_send_bytes;
    dev->send_char = HAL_send_char;
    dev->read_char = HAL_read_char;
    dev->send_str = HAL_send_str;
}

void HAL_init_uart(UART_DEV *dev, uart_port_t port, int baud, int rx_pin, int tx_pin)
{
    uart_config_t uart_conf = {
        .baud_rate = baud,
        .data_bits = UART_DATA_8_BITS,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_DISABLE,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(port, &uart_conf));
    ESP_ERROR_CHECK(uart_set_pin(port, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    if (port == UART0) 
        ESP_ERROR_CHECK(uart_driver_install(port, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 10, &uart0_queue, 0));
    else if (port == UART1) 
        ESP_ERROR_CHECK(uart_driver_install(port, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 10, &uart1_queue, 0));
#if SOC_UART_HP_NUM > 2
    else if (port == UART2)  
        ESP_ERROR_CHECK(uart_driver_install(port, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 10, &uart2_queue, 0));
#endif
    dev->uart_num = port;
}

void HAL_send_char(UART_DEV *dev, char ch)
{
    if(UART_TIMEOUT != -1)
    {
        if((uint8_t)ch != 0xff)  uart_write_bytes(dev->uart_num, &ch, 1);
        return;
    }
       uart_write_bytes(dev->uart_num, &ch, 1);  
}

void HAL_send_str(UART_DEV *dev, char* str)
{
    if(UART_TIMEOUT != -1)
    {
    if((uint8_t)str[0] != 0xff)
    {
    uart_tx_chars(dev->uart_num,str,strlen(str));
    uart_flush(dev->uart_num);
    return;
    }
    }
    uart_tx_chars(dev->uart_num,str,strlen(str));
    uart_flush(dev->uart_num);
}

void HAL_send_bytes(UART_DEV *dev, uint8_t *data, size_t len)
{
    if(UART_TIMEOUT != -1)
    {
        if(data != 0xff)
        {
        uart_write_bytes(dev->uart_num,data,len);
        return ;
        }

    }
    uart_write_bytes(dev->uart_num,data,len);
 
}

int8_t HAL_read_bytes(UART_DEV *dev)
{
    uint8_t rx_data;
    int length = uart_read_bytes(dev->uart_num, &rx_data, 1, UART_TIMEOUT);
    if (length > 0) 
    {
        return rx_data; 
    }
    else
    {
        return EOF; 
    }
}


char HAL_read_char(UART_DEV *dev)
{
    char rx_data;
    int length = uart_read_bytes(dev->uart_num, &rx_data, 1, UART_TIMEOUT);
    if (length > 0 )
        return rx_data;
    else 
        return EOF;
}
