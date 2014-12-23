(function($) {

  "use strict";

  Drupal.behaviors.tingVisualRelation = {
  	attach: function(context, settings) {
      relvis.init({
        apiUrl: settings.tingVisualRelation.apiUrl,
        logUrl: settings.tingVisualRelation.logUrl,
        clickHandle: function(o) {
          if (typeof(o.id) !== 'undefined') {
            window.open('/ting/object/' + o.id);
          }
        }
      });
  	}
  }

}(jQuery));
