#include <FreeRTOS.h>
#include <task.h>
#include <console.h>

#define CHALLENGE_TYPE_EMPTY   (0x10)
#define CHALLENGE_TYPE_ADD     (0x24)
#define CHALLENGE_TYPE_RESULT  (0x32)
#define CHALLENGE_TYPE_DELAY   (0x43)
#define CHALLENGE_TYPE_TIMEOUT (0x51)
#define CHALLENGE_TYPE_LOG     (0x60)

static uint8_t Challenge_typeID;
static uint8_t Challenge_dataLength;
static uint8_t Challenge_pendingData;
static uint8_t Challenge_dataCounter;
static uint8_t Challenge_dataBuffer[10];
static TickType_t Challenge_start;
static TickType_t Challenge_timeOut;

static void Challenge_HandlerEmpty();
static void Challenge_HandlerAdd();
static void Challenge_HandlerResult();
static void Challenge_HandlerDelay();
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

    if(Challenge_pendingData == 1)
    {  
       if(Challenge_dataCounter < Challenge_dataLength)
       {
           Challenge_dataBuffer[    Challenge_dataCounter] = data;
           Challenge_dataCounter++;
       }       
       
    }
    else
    {
        Challenge_typeID = data;
        Challenge_dataLength = data & 0x0F;
    } 

}

/**
 * Initialize challenge. This is called once on startup, before any interrupts can occur.
 */
void challenge_init() {
    Challenge_start = xTaskGetTickCount();
    Challenge_timeOut = 0xFFFFFFFF;
    Challenge_typeID = 0xFF; // NOT INIT
    Challenge_dataLength = 0xFF; // NOT INIT
    Challenge_pendingData = 0;
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
    while(1)
    {
        TickType_t elapsed = xTaskGetTickCount() - Challenge_start;
        //if(elapsed > Challenge_timeOut) Challenge_typeID = CHALLENGE_TYPE_TIMEOUT;
        //console_print("ERROR: Hola 0x%x\n",Challenge_pendingData );

        vTaskDelay(10);    

        switch(Challenge_typeID)
        {
            case CHALLENGE_TYPE_EMPTY:
            { 
                Challenge_pendingData = 1;                      
                Challenge_HandlerEmpty();
                break;
            }
            case CHALLENGE_TYPE_ADD:
            {
                Challenge_HandlerAdd();
                break;
            }
            case CHALLENGE_TYPE_RESULT:
            {
                
                break;
            }
            case CHALLENGE_TYPE_DELAY:
            {               
                Challenge_HandlerDelay();
                break;
            }
            case CHALLENGE_TYPE_TIMEOUT:
            {
                Challenge_HandlerTimeOut();
                break;
            }
            case CHALLENGE_TYPE_LOG:
            {
                break;
            }
            default:
            {
                //Challenge_pendingData = 0;
                //Error
            }
        }
        
    }
    // TODO: insert awesome stuff here

}

void Challenge_HandlerEmpty()
{
    Challenge_pendingData = 0;
    Challenge_dataCounter = 0;
    uint8_t message[] = {CHALLENGE_TYPE_EMPTY};
    send(message, sizeof(message));
}

void Challenge_HandlerAdd()
{
    uint8_t response[3];   

    if(Challenge_dataCounter < Challenge_dataLength)
    {
        Challenge_pendingData = 1;
    }
    else
    {
        Challenge_pendingData = 0;
        Challenge_dataCounter = 0;
        
        uint16_t r = (Challenge_dataBuffer[0] + Challenge_dataBuffer[2]) * 256 + Challenge_dataBuffer[1] + Challenge_dataBuffer[3];

        response[0] = 0x32;
        response[1] = r>>8;
        response[2] = r;

        //console_print("Result en ADD %x %x %x\n", response[0], response[1], response[2]);
        send(response, sizeof(response));      
    }    

}

void Challenge_HandlerResult()
{

}

void Challenge_HandlerDelay()
{
    //console_print("ERROR: Hola 3\n");
    uint8_t delay[Challenge_dataLength];
    uint8_t response[4];

    if(Challenge_dataCounter < Challenge_dataLength)
    {
        Challenge_pendingData = 1;
        delay[Challenge_dataCounter] = Challenge_dataBuffer[Challenge_dataCounter];
    }
    else
    {
        Challenge_timeOut = (Challenge_dataBuffer[1]) * 256 + Challenge_dataBuffer[0];      
    }
}

void Challenge_HandlerTimeOut()
{
    Challenge_pendingData = 0;
    Challenge_dataCounter = 0;
    uint8_t response[4];
    response[0] = 0x51;
    send(response, 1);

}

void Challenge_HandlerLog()
{

}
