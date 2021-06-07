(function ($) {
	Drupal.behaviors.menuglyphicon = {
		    attach: function (context, settings) {
		    	var bouton = $( ".dropdown .toggle-custom" );
		    	bouton.click(function(event){
		    		btn = $(this);
		    		if ( btn.attr("aria-expanded") == 'true' ) {
		    			btn.removeClass("glyphicon-minus");
			    		btn.addClass("glyphicon-plus");
			    	}else{
			    		btn.removeClass("glyphicon-plus");
		    			btn.addClass("glyphicon-minus");
			    	}
		    	});
		    }
	}
})(jQuery);


