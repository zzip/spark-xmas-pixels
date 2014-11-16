    var coreId = '';

    var eventCb = function(data) {
      console.log("Event: " + data);
    }

    var attrsCb = function(err, data) {
      if (err) {
        console.log('An error occurred while calling function on core:', err);
        addAlert({msg: '<strong>Error!</strong> An error occurred while calling function on core ' + err,
                  type: 'danger'});
      } else {
        console.log('Core received function successfully:', data);
      }
    }

    var settingsCb = function(err, data) {
      if (err) {
        console.log('An error occurred while getting variable:', err);
        addAlert({type: 'danger',
                  msg: '<strong>Error!</strong> An error occurred while getting variable:' + err});
      } else {
        console.log(data.name + "=" + data.result);
        updateUI(data);
      }
    }

    function sendToCore(function_name, args) {
      spark.callFunction(coreId, function_name, args, attrsCb);
    }

    function sendHandleParams(key,value) {
      cmd = key + ':' + value;
      sendToCore('handleparams', cmd);
    }


    function subscribeToCores() {
      // Sparkcore Connection
      var url = "https://api.spark.io/v1/devices/events?access_token=" + spark.accessToken;
      var es = new EventSource(url);
      console.log("Listening for events");

      es.addEventListener('new-cmd', function(e){
          evt = JSON.parse(e.data);
          console.log("SERVER SIDE EVENT");
          console.log(evt);
          var post  = {name: 'SYSTEM',data: evt['data'], ttl: evt['ttl'], published_at: evt['published_at'],core_id: evt['coreid']};

          //update sse paragraph
          $('#sse').text('Your Core said ' + evt['data'] + ' at ' + evt['published_at']);

          //highlight effect button for the currently running effect
          $('button[type="submit"]').removeClass('btn-info');
          $('button[type="submit"]').removeClass('btn-success');
          $('button[type="submit"][value="' + evt['data'] + '"]').addClass('btn-success');
      }, false);

    };

    function getSettings(device) {
      device.getVariable('brightness', settingsCb);
      device.getVariable('delay', settingsCb);
      device.getVariable('effect', settingsCb);
      device.getVariable('red', settingsCb);
      device.getVariable('green', settingsCb);
      device.getVariable('blue', settingsCb);
    }

    function enumerateCores() {

      var devicesPr = spark.listDevices();


      devicesPr.then(
        // We get an array with devices back and we list them
        function(devices){
          console.log('API call List Devices: ', devices);
          // When you get the list of devices each of them has all retrieved attributes
          // inside an object with the same name `core.attributes`, also the attributes
          // are aliased to the device main object like so `core.attributes.id === core.id`
          var core = devices[0];
          coreId = devices[0].id;

          console.log('core own attributes:', core);
          console.log('core.attributes:', core.attributes);
          if (core.attributes.connected) {
            spark.getAttributes(coreId, attrsCb);
            getSettings(core);
          } else {
            console.log('CORE NOT CONNECTED');
            addAlert({msg: '<strong>Warning!</strong> Your Spark Core is not connected. Connect and reload this page.'});
          }
          //Get your devices events
          //var evl = spark.getEventStream(false, 'mine', false);
          //console.log(evl);

        },
        function(err) {
          console.log('API call failed: ', err);
        }
      );

    }

    function setupAfterSparkLogin() {
      localStorage.setItem('sparkToken',spark.accessToken);
      $('#spark-login').toggle();
      $('#btnLogout').removeClass('hidden');
      subscribeToCores();
      enumerateCores();
    }

    var sparkToken = localStorage.getItem('sparkToken');
    if (sparkToken) {

      addAlert({msg: 'Using your locally stored acess token to log you in, please hold on.',
               type: 'success', id: 'loginAlert', closeButton: false});

      spark.on('login', function() {
        console.log("Logged In");
        $('#loginAlert').alert('close');
        setupAfterSparkLogin();
      });

      spark.login({accessToken: sparkToken});

    } else {

      sparkLogin(function() {
        setupAfterSparkLogin();
      });

    }

