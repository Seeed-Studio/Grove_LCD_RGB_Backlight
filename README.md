Grove - LCD RGB Backlight
---------------------------------------------------------

[![Grove - LCD RGB Backlight](https://raw.githubusercontent.com/SeeedDocument/Grove_LCD_RGB_Backlight/master/images/intro.jpg)](http://www.seeedstudio.com/depot/grove-lcd-rgb-backlight-p-1643.html?cPath=34_36)


<br>
Done with the tedious mono color backlight? This Grove - LCD RBG Backlight enables you to set the color to whatever you like via the simple and concise Grove interface. It takes I2C as the communication method with your microcontroller. The number of pins required for data exchange and backlight control shrinks from ~10 to 2, relieving IOs for other challenging tasks. In addition, Grove - LCD RGB Backlight supports user-defined characters. Want to get a love heart or another custom character? Just take advantage of this feature and design it!



<br>
## Usage:

This is an Arudino Library. It include a .h file, a .cpp file and some examples. Through these examples, you can quickly master the use of Grove - LCD RGB Backlight.

The in the following, we will introduce some functions which are used very intuitively. 


### Initialization
Before we use this lcd, we should initialize it. You can use this function:

    lcd.begin(16, 2);

This means that this lcd has 16 columns and 2 rows.


<br>
### Change Color of Backlight
One of Grove - LCD RGB Backlight's most important feature is changing the backlight color. It's very simple; just use the folowing function:

    void setRGB(int r, int g, int b);


<br>
### Clear Display

You can clear the display by this function:

    void clear();

<br>
### Turn on and turn off display

    void noDisplay();			// turn off display
    void display();				// turn on display

<br>
### Blink

    void noBlink();
    void blink();

<br>
### Cursor

    void noCursor();
    void cursor();

<br>
### Blink LED Backlight

    void noBlinkLED();
    void blinkLED();


<br>
For more information, please refer to [wiki page](http://www.seeedstudio.com/wiki/Grove_-_LCD_RGB_Backlight).

    
----

This software is written by loovee([luweicong@seeedstudio.com](luweicong@seeedstudio.com "luweicong@seeedstudio.com")) for seeed studio<br>
and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt for more information.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>



[![Analytics](https://ga-beacon.appspot.com/UA-46589105-3/Grove_LCD_RGB_Backlight)](https://github.com/igrigorik/ga-beacon)



