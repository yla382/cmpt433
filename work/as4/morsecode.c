#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
#include <linux/leds.h>

#define MY_DEVICE_FILE  "morse-code"
#define FIFO_SIZE 1024
#define LED_DOT_TIME_ms 200
#define LED_DASH_TIME_ms 600

DEFINE_LED_TRIGGER(ledtrig_morsecode);

//Morse code look up table
static char* morsecode[] = {
		".-", // A
		"-...", // B
		"-.-.", // C
		"-..", // D
		".", // E
		"..-.", // F
		"--.", // G
		"....", // H
		"..", // I
		".---", // J
		"-.-", // K
		".-..", // L
		"--", // M
		"-.", // N
		"---", // O
		".--.", // P
		"--.-", // Q
		".-.", // R
		"...", // S
		"-", // T
		"..-", // U
		"...-", // V
		".--", // W
		"-..-", // X
		"-.--", // Y
		"--..", // Z
};


/*
Helper function to check if the character is a whitespace
input: char
output: bool
*/
static bool isWhiteSpace(char ch)
{
	return (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t');
}


/*
Helper function to check if character is alphabet
input: char
output: bool
*/
static bool isAlphabet(char ch)
{
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}


/*
Blinks LED for one dot time (200ms)
input : void
output: void
*/
static void my_led_dot(void)
{
	led_trigger_event(ledtrig_morsecode, LED_FULL);
	msleep(LED_DOT_TIME_ms);
	led_trigger_event(ledtrig_morsecode, LED_OFF);
}


/*
Blinks LED for three dot time (600ms)
input : void
output: void
*/
static void my_led_dash(void)
{
	led_trigger_event(ledtrig_morsecode, LED_FULL);
	msleep(LED_DASH_TIME_ms);
	led_trigger_event(ledtrig_morsecode, LED_OFF);
}


/*
Function to create a led trigger specific to morse-code
input : void
output: void
*/
static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple("morse-code", &ledtrig_morsecode);
}


/*
Function to remove the trigger
input : void
output: void
*/
static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(ledtrig_morsecode);
}


/******************************************************
 * Callbacks
 ******************************************************/
static DECLARE_KFIFO(text_input, char, FIFO_SIZE);


static ssize_t my_read(struct file *file, char *buff, size_t count, loff_t *ppos)
{
	// Pull all available data from fifo into user buffer
	int num_bytes_read = 0;
	if (kfifo_to_user(&text_input, buff, count, &num_bytes_read)) return -EFAULT;

	return num_bytes_read;  // # bytes actually read.d.
}
 

static ssize_t my_write(struct file *file, const char *buff, size_t count, loff_t *ppos)
{
	int i, j, whiteSpaceCount, ascii, ascii_flag;
	char *morse;
	bool isCharWhiteSpace;
	whiteSpaceCount = 0;

	// Push data into fifo, one byte at a time (with delays)
	for (i = 0; i < count; i++) {
		char ch;
		//copy value from usersapce to ch
		if (copy_from_user(&ch, &buff[i], sizeof(ch))) return -EFAULT;

		isCharWhiteSpace = isWhiteSpace(ch);
		
		//if input string starts with whitespace skip loop
		if(i == 0 && isCharWhiteSpace) continue;
		//if input string end with whitespace skip loop
		if(i == count - 1 && isCharWhiteSpace) continue;

		//Insert character in morse-code into the KFIFO if character is alphabet
		if(!isCharWhiteSpace) {
			// Demo how to insert data from kernel-mode into fifo
			if(!isAlphabet(ch)) continue;
            ascii = ch;
            ascii_flag = 0; // to not double subtract
            if (ascii >= 65 && ascii <= 90) { //check for upper case alphabet
                ascii = ch - 65;
                ascii_flag = 1;

            } else if (ascii >= 97 && ascii <= 122 && ascii_flag == 0) { //check for lower case alphabet
                ascii = ch - 97;
                ascii_flag = 1;
            }
			morse = morsecode[ascii];

			int morse_size;
			int k;
			morse_size = 0;
			for(k = 0; morse[k] != '\0'; k++) {
				morse_size++;
			}

			for(j = 0; j < morse_size; j++) {
				if (!kfifo_put(&text_input, morse[j])) return -EFAULT;
				if (morse[j] == '.') {
                    my_led_dot();
                } else if (morse[j] == '-') {
                    my_led_dash();
                }

                if(j < morse_size - 1){
                	msleep(LED_DOT_TIME_ms);
                }
			}

			if(i < count - 2) {
				msleep(LED_DOT_TIME_ms * 3);
			}

            printk(KERN_INFO " %c\n", ch);

			if (!kfifo_put(&text_input, ' ')) return -EFAULT;

			whiteSpaceCount = 0;

		} else {
			if(whiteSpaceCount == 0) {
				if (!kfifo_put(&text_input, ' ')) return -EFAULT;
				if (!kfifo_put(&text_input, ' ')) return -EFAULT;
				if(i < count - 2) msleep(LED_DOT_TIME_ms * 4);
			}
			whiteSpaceCount++;
		}
	}

	//testing purposes
	if (!kfifo_put(&text_input, '\n')) return -EFAULT;

	// Return # bytes actually written.
	*ppos += count;
	return count;
}

/******************************************************
 * Misc support
 ******************************************************/
// Callbacks:  (structure defined in /linux/fs.h)
struct file_operations my_fops = {
	.owner    =  THIS_MODULE,
	.read     =  my_read,
	.write    =  my_write,
};


// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,             // /dev/.... file.
		.fops     = &my_fops                    // Callback functions.
};

static int __init morseCodeDriver_init(void)
{
	int ret;
	printk(KERN_INFO "----> Morse Code driver init()\n");
	//Initialize KFIFO
	INIT_KFIFO(text_input);
	//Create driver in /dev
	ret = misc_register(&my_miscdevice);
    led_register();
	return ret;
}


static void __exit morseCodeDriver_exit(void)
{
	printk(KERN_INFO "<---- Morse Code driver exit().\n");
	//Remove the driver from /dev
	misc_deregister(&my_miscdevice);

    // LED:
	led_unregister();
}


module_init(morseCodeDriver_init);
module_exit(morseCodeDriver_exit);

MODULE_AUTHOR("Yoonhong Lee & Jusung Lee");
MODULE_DESCRIPTION("CMPT433 Assignment4: Morse Code Driver");
MODULE_LICENSE("GPL"); // Important to leave as GPL.