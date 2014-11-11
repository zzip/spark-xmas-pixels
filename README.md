Simple mashup to control a WS2811/WS2812B led strip connected to a [Spark Core](http://spark.io/) through your browser.

Requirements
------------
* [Spark Core](http://spark.io/)
* [Ruby](http://ruby-lang.org/)


Instructions
------------

### Spark part
1. Copy the spark_src/led-strip.ino file into the Spark IDE as a new app.  
2. Be sure to add the NeoPixel library for that app.  
3. Set the `PIXEL_COUNT` and the `CONVERT_AT` values.  
4. Verify and flash to your core(s)

### Ruby part

1. `gem install bundler`

2. `bundle install`

3. `copy config.yml.example config.yml`

4. edit config.yml and set your authentication_token and device_id(s)

5. `ruby server.rb`

6. open up a browser and surf to [http://localhost:4567/](http://localhost:4567/)

