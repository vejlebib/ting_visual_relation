(function($) {

  "use strict";

  Drupal.behaviors.tingVisualRelation = {
  	attach: function(context, settings) {
      relvis.init({
        apiUrl: settings.tingVisualRelation.apiUrl,
        logUrl: settings.tingVisualRelation.logUrl,
        clickHandle: function(o) {
          if (typeof(o.id) !== 'undefined') {
            // Regex that matches a datawell-pid
            var regex = /^\d{6}-{1}\D+:{1}\d+$/;
            // If this a datawell object: Show it
            if (regex.test(o.id)) {
              window.open('/ting/object/' + o.id);
            }
            // Otherwise it's a term or a name: Use it in a search
            // TODO: Should use term.subject or term.author in search.
            else {
              window.open('/search/ting/' + o.id);
            }
          }
        }
      });
  	}
  }

}(jQuery));
