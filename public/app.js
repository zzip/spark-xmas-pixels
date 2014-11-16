  function updateUI(data) {
    switch (data.name) {
      case 'delay':
        $('input[name="'+ data.name +'"]')[0].value = data.result;
        break;
      case 'brightness':
        $('input[name="'+ data.name +'"]').each(function(){
          //console.log($(this));
          $(this).value = data.result;
          $(this).slider('setValue', data.result);
        });
        break;
      case 'effect':
        $('button[type="submit"][value="' + data.result + '"]').addClass('btn-success');
        break;
      case 'red':
      case 'green':
      case 'blue':
        $(".color-picker").each(function(){
          var c = $(this).spectrum("get");
          var ca = c.toRgb();
          //console.log(c);
          ca[data.name[0]] = data.result;
          $(this).spectrum("set", ca);
        });
        break;
    }

  }

  function addAlert( args ) {
    var defaults = {
      type: 'warning',
      msg: '',
      id: 'alert',
      closeButton: true
    };

    args = $.extend({}, defaults, args);

    if (args.closeButton) {
      closeHtml = '<button type="button" class="close" data-dismiss="alert">' +
        '<span aria-hidden="true">&times;</span>' +
        '<span class="sr-only">Close</span></button>'
    } else { closeHtml = '' }

    html = '<div class="alert alert-dismissible fade in  alert-' +
            args.type + '" id="' + args.id +'" role="alert">' +
            closeHtml +
            args.msg + '</div>';
    $('#alerts').append(html);
  }

  $(document).ready(function(){
    //farbtastic color picker
    //$(function () {
    //    $('#colorpicker1').farbtastic('#inputColor');
    //});

    //setup spectrum color picker
    $(".color-picker").spectrum({
      //color: "#f00",
      //flat: bool,
      showInput: true,
      showInitial: true,
      allowEmpty: false,
      showAlpha: false,
      showButtons: false,
      localStorageKey: 'spectrum.xmas',
      showPalette: true,
      showPaletteOnly: false,
      togglePaletteOnly: false,
      showSelectionPalette: true,
      hideAfterPaletteSelect: true,
      clickoutFiresChange: true,
      containerClassName: 'color-picker-spectrum',
      preferredFormat: 'hex',
      //maxSelectionSize: int,
      change: function(color) {
        sendHandleParams('c', color.toHexString());
      }
    });

    //setup bootstrap slider controls
    $('#inputAdjustBrightness, #inputBrightness').slider();

    //handle brightness changes
    $("#inputAdjustBrightness, #inputBrightness").on("slide", function(slideEvt) {
      $('.inputBrightnessVal').text('Brightness (' + slideEvt.value + ')');
    });
    $('#inputAdjustBrightness, #inputBrightness').on('slideStop', function(slideEvt) {
      $('.inputBrightnessVal').text('Brightness (' + slideEvt.value + ')');
      sendHandleParams('B', slideEvt.value);
    })

    //handle delay/wait time change
    $('#inputDelay').on('change', function(ev ) {
      console.log("changing delay");
      sendHandleParams('w', ev.target.value);
    })

    //handle buttons for setting effects
    $('#genericForm button[type="submit"]').click( function(event){
      event.preventDefault();
      var btn = event.currentTarget;
      $(btn).addClass('btn-info');
      sendHandleParams('E', btn.value);
    });

    //handle buttons for setting effects
    $('#sequenceForm button[type="submit"]').click( function(event){
      event.preventDefault();
      var seq = $('#inputSequence');
      sendToCore('setsequence', seq[0].value);
    });

    $('#btnLogout').click( function(event){
      localStorage.removeItem('sparkToken');
      $('#btnLogout').addClass('hidden');
    });

  });
