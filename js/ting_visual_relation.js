(function($) {

  "use strict";

  Drupal.behaviors.tingVisualRelation = {
  	attach: function(context, settings) {
      relvis.init({
        apiUrl: settings.tingVisualRelation.apiUrl,
        logUrl: settings.tingVisualRelation.logUrl,
        clickHandle: function(o) {
          console.log('click', o);
        }
      });
  	}
  }

}(jQuery));
