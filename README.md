Simple mashup to control a WS2811/WS2812B led strip connected to a [Spark Core](http://spark.io/) through your browser.

Requirements
------------
* [Spark Core](http://spark.io/)
* ~~[Ruby](http://ruby-lang.org/)~~

Instructions
------------

### Spark part

If you have the [spark-cli tool](https://github.com/spark/spark-cli) installed : 

1. Open `spark_src/spark-xmas-pixels.ino` and change the `PIXEL_COUNT` and the `CONVERT_AT` values.
2. `spark flash DEVICE_ID spark_src` (replace `DEVICE_ID` with your core device id)

If instead you want to use the [Spark WEB IDE](http://spark.io/build) :

1. Open `spark_src/spark-xmas-pixels.ino` and change the `PIXEL_COUNT` and the `CONVERT_AT` values.  
2. Copy the `spark_src/spark-xmas-pixels.ino` file into the Spark IDE as a new app.
3. Change `#include "neopixel.h"` to `#include "neopixel/neopixel.h"`
4. Be sure to add the NeoPixel library for that app.
5. Verify and flash to your core(s)

### Browser part

1. Open `public/index.html` in a browser.
2. Press the Login button and log in to Particle.io
3. The id of your Particle Core is fetched from Particle.io
4. Play with the lights

~~### Ruby part~~

1. ~~`gem install bundler`~~
2. ~~`bundle install`~~
3. ~~`copy config.yml.example config.yml`~~
4. ~~edit config.yml and set your authentication_token and device_id(s)~~
5. ~~`ruby server.rb`~~
6. ~~open up a browser and surf to [http://localhost:4567/](http://localhost:4567/)~~
