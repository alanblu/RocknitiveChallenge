#include <FreeRTOS.h>
#include <task.h>
#include <console.h>

#define CHALLENGE_TYPE_EMPTY   (0x10)
#define CHALLENGE_TYPE_ADD     (0x24)
#define CHALLENGE_TYPE_RESULT  (0x32)
#define CHALLENGE_TYPE_DELAY   (0x43)
#define CHALLENGE_TYPE_TIMEOUT (0x51)
#define CHALLENGE_TYPE_LOG     (0x60)
#define CHALLENGE_BUFFER_SIZE  (16  )

static uint8_t Challenge_buffer_data[CHALLENGE_BUFFER_SIZE];

static uint8_t Challenge_dataCounter;
static TickType_t Challenge_start;
static TickType_t Challenge_timeOut;

static void Challenge_HandlerEmpty(uint8_t data[], uint16_t lenght, uint16_t idx );
static void Challenge_HandlerAdd(uint8_t data[], uint16_t lenght, uint16_t init) ;
static void Challenge_DumpData(uint8_t data[], uint16_t lenght, uint16_t init);
static void Challenge_HandlerResult();
static void Challenge_HandlerDelay(uint8_t data[], uint16_t lenght, uint16_t idx);
static void Challenge_HandlerTimeOut();
static void Challenge_HandlerLog();

/**
 * Call this to "send" data over the (simulated) serial interface.
 * @param message message buffer
 * @param length length of the message
 */
void send(uint8_t* message, uint32_t length);

/**
 * This will get called for each byte of data received.
 * @param data received byte
 */
void receive_ISR(uint8_t data) {

    if(Challenge_dataCounter>CHALLENGE_BUFFER_SIZE) Challenge_dataCounter = 0;

    Challenge_buffer_data[Challenge_dataCounter] = data;
    Challenge_dataCounter++;

}

/**
 * Initialize challenge. This is called once on startup, before any interrupts can occur.
 */
void challenge_init() {
    Challenge_start = xTaskGetTickCount();
    Challenge_timeOut = 0xFFFFFFFF;
    Challenge_dataCounter = 0;
}



/**
 * Main function for the coding challenge. Must not return.
 *
 * Please use the following API functions if necessary:
 *
 * print string to stdout
 * console_print("format %d", 123);
 *
 * millisecond delay
 * vTaskDelay(123);
 *
 * get elapsed milliseconds
 * TickType_t ms = xTaskGetTickCount();
 */
void challenge_run() 
{
    uint32_t idx = 0;

    while(1)
    {
        if(idx >= CHALLENGE_BUFFER_SIZE) idx = 0;

        TickType_t elapsed = xTaskGetTickCount() - Challenge_start;
        if(elapsed > Challenge_timeOut) Challenge_HandlerTimeOut();
        //console_print("ERROR: Hola 0x%x\n",Challenge_pendingData );

        vTaskDelay(100);    
        

        switch(Challenge_buffer_data[0])
        {
            case CHALLENGE_TYPE_EMPTY:
            {                       
                Challenge_HandlerEmpty(&Challenge_buffer_data[0], 1, 0);
                break;
            }
            case CHALLENGE_TYPE_ADD:
            {
                Challenge_HandlerAdd(&Challenge_buffer_data[0], 4, 0);
                break;
            }
            case CHALLENGE_TYPE_RESULT:
            {
                
                break;
            }
            case CHALLENGE_TYPE_DELAY:
            {               
                Challenge_HandlerDelay(&Challenge_buffer_data[0], 3, 0);
                break;
            }
            case CHALLENGE_TYPE_TIMEOUT:
            {
                break;
            }
            case CHALLENGE_TYPE_LOG:
            {
                break;
            }
            default:
            {
                
                //console_print("Result en ADD en challenge 0x%x \n", Challenge_buffer_data[idx] );
                Challenge_DumpData(&Challenge_buffer_data[0], 1, 0);
                //Challenge_pendingData = 0;
                //Error
            }
        }


    }
    // TODO: insert awesome stuff here

}

void Challenge_HandlerEmpty(uint8_t data[], uint16_t lenght, uint16_t idx )
{
    Challenge_DumpData(data, lenght, idx);
    uint8_t message[] = {CHALLENGE_TYPE_EMPTY};
    send(message, sizeof(message));
}

void Challenge_HandlerAdd(uint8_t data[], uint16_t lenght, uint16_t idx )
{
    uint8_t response[3];   
        
    uint16_t r = (data[1] + data[3]) * 256 + data[2] + data[4];

    response[0] = 0x32;
    response[1] = r>>8;
    response[2] = r;

    Challenge_DumpData(data, lenght, idx);
    send(response, sizeof(response));
}

void Challenge_HandlerResult()
{

}

void Challenge_HandlerDelay(uint8_t data[], uint16_t lenght, uint16_t idx)
{
    uint8_t response[3];   
        
    uint16_t r = data[1] * 256 + data[2];

    response[0] = 0x32;
    response[1] = r>>8;
    response[2] = r;

    Challenge_DumpData(data, lenght, idx);

    Challenge_timeOut =   r;   
}

void Challenge_HandlerTimeOut()
{
    uint8_t response[1];
    response[0] = CHALLENGE_TYPE_TIMEOUT;
    send(response, 1);
}

void Challenge_HandlerLog()
{

}

void Challenge_DumpData(uint8_t data[], uint16_t lenght, uint16_t init)
{
    /*console_print("\n");
    console_print("Result en ADD\n");
    for(int idx2 = 0; idx2 < CHALLENGE_BUFFER_SIZE; idx2++ )
    {
        console_print(" %x ", data[idx2]);       
    }
    console_print("\n");*/
    
    Challenge_dataCounter=-lenght;
    for(int idx = init; idx < CHALLENGE_BUFFER_SIZE - lenght; idx++ )
    {
        data[idx] = data[idx + lenght + 1];
    }

    /*console_print("Result despues ADD \n");
    for(int idx2 = 0; idx2 < CHALLENGE_BUFFER_SIZE; idx2++ )
    {
        console_print(" %x ", data[idx2]);       
    }
    console_print("\n");*/

}
