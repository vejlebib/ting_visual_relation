(function($) {

  "use strict";
  
  Drupal.behaviors.tingVisualRelation = {
  	attach: function(context, settings) {
  		// Test jsonp:
			var script = document.createElement('script');
			script.type = 'text/javascript';
      //script.src = '/ting-visual-relation/get-ting-object/870971-tsart:84419214?callback=test';
			script.src = '/ting-visual-relation/get-ting-object/870970-basis:26917921?callback=test';
			var head = document.getElementsByTagName('head')[0];
			head.appendChild(script);
      // Normal ajax call test:
  		/*$.ajax({
  			url: "/ting-visual-relation/get-ting-object",
  			success: function(data) {
  				alert("success");
  			}
  		}); */
  	}
  }

}(jQuery));
