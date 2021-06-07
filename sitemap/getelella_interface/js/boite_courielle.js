(function ($) {
  Drupal.behaviors.boitecourrielle = {
    attach: function (context, settings) {
    	new PNotify({
    	    title: 'Regular Notice',
    	    text: 'Check me out! I\'m a notice.'
    	});
    	window.provenance = 'recu';
    	
    	$(document).on('click', '.email-list-item', function(){
    		$(".inbox-body").html('<div class="loader"></div>');
    			var nid = $(this).children("input[name='nid'][type='hidden']").val();
    			$(document).affiher_courriel(nid);
    	});
    	
    	$('input[name=provenance]').on('change', function() {
    		$("#mail_list_column").html('<div class="loader"></div>');
    		$(".inbox-body").html('<div class="loader"></div>');
    		window.provenance = $('input[name=provenance]:checked').val();
    		$(document).reload_email();
    		   new PNotify({
    			    title: 'Regular Notice',
    			    text: 'Check me out! I\'m a notice.'
    			 });
    	});
    	if(!window.raffraichir_premier_courriel){
    		$(document).raffraichir_premier_courriel();
    		window.raffraichir_premier_courriel = 1;
    	}
    	
    	if(!window.rafraichir_liste_interval){
    		window.rafraichir_liste_interval = setInterval($(document).rafraichir_liste, 600000, 1);
    	}
    }
  };
  

  
  $.fn.affiher_courriel = function(nid) {
		var adresse = $("input[name=adresse_globale]").val();
	  jQuery(".inbox-body").load("/email/get/ajax/" + nid + "?destination_force=" + adresse);
	}

  $.fn.reload_email = function(recharger) {
	var nid_boite_courielle = $("input[name=nid_boite_courriel]").val();
	var cle_provenance;
	if(window.provenance=='recu'){
		cle_provenance = 0;
	}else if(window.provenance=='pending'){
		cle_provenance = 3;
	}else{
		cle_provenance = 1;
	}
	$("#mail_list_column .view-content").removeClass("gd-infinite-scroll-initialized");
	var paramfin;
	if(recharger){
		paramfin = "/siGet/1";
	}else{
		paramfin = "/siGet/0";
	}
	jQuery("#mail_list_column").load("/dashboard/inbox/messageindex/" + nid_boite_courielle + "/provenance/" + cle_provenance + paramfin, function() {
		Drupal.attachBehaviors('#mail_list_column');
		$(document).raffraichir_premier_courriel();
	});
  }
  
  $.fn.raffraichir_premier_courriel = function(){
	var nidPremier = $(".mail_list_column a.email-list-item:first input[name=nid]").val();
	$(document).affiher_courriel(nidPremier);
  }
  
  $.fn.rafraichir_liste = function(siFeed){
	$(document).reload_email(siFeed);
  }

})(jQuery);
