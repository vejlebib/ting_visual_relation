(function($) {

  "use strict";

  Drupal.behaviors.tingVisualRelation = {
  	attach: function(context, settings) {
      relvis.init({
        apiUrl: Drupal.settings.tingVisualRelation.apiUrl,
        logUrl: Drupal.settings.tingVisualRelation.logUrl,
        clickHandle: function(o) {
          console.log('click', o);
        }
      });
  	}
  }

}(jQuery));
