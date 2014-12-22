(function($) {

  "use strict";

  Drupal.behaviors.tingVisualRelation = {
  	attach: function(context, settings) {
      relvis.init({
        apiUrl: settings.tingVisualRelation.apiUrl,
        logUrl: settings.tingVisualRelation.logUrl,
        clickHandle: function(o) {
          console.log('click', o);
          if (typeof(o.id) !== 'undefined') {
            var a = document.createElement('a');
            a.href = '/ting/object/' + o.id;
            a.target = '_blank';
            var evt = new window.MouseEvent('click', {
              view: window,
              bubbles: true,
              cancelable: true
            });
            a.dispatchEvent(evt);
          }
        }
      });
  	}
  }

}(jQuery));
