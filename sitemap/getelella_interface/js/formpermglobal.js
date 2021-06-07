(function ($) {
  Drupal.behaviors.testinterface = {
    attach: function (context, settings) {
    	
    	$(document).on('click', 'tr > td > div.caca', function(){
    		$(this).tescaca();
    	});
    	
    	$("tr > td > div.caca").once().each(function() {
    		entete = $("#permissions > thead").html();
    		$( this ).tescaca();
    		$( this ).hover(function() {
    			  $(this).css("cursor","pointer");
    		});
    	});
    }
  
  };
  
  $.fn.tescaca = function(){
	  //alert($(this).html());
	  element = $(this).parents().parents();
	  cboxOverlayDispl = $("#cboxOverlay").css("display");
	  colorboxDsip = $("#colorbox").css("display");
	  pnotifydisp = $(".ui-pnotify").css("display");
	  adminmenuDisp = $("#admin-menu").css("display");
		while(element.next().children("td").children(".caca").length == 0 &&
			element.next().is("#permissions tbody tr")
		){
			element = element.next();
			element.toggle();
			$("#cboxOverlay").css({"display": cboxOverlayDispl});
			$("#colorbox").css({"display": colorboxDsip});
			$(".ui-pnotify").css({"display": pnotifydisp});
			$("#admin-menu").css({"display": adminmenuDisp});
		}
  };
})(jQuery);
