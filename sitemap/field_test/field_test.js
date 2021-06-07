(function($){
  
  Drupal.field_test = Drupal.field_test ? Drupal.field_test : {};
  
  Drupal.field_test.update = function (event, ui) {
    var items = [];
    var ec = $(event.target).attr("data-ec");
    $(".entityreference-dragdrop-selected[data-ec=" + ec + "] li").each(function(index) {
      items.push($(this).attr('data-id'));
    });
    $("input.entityreference-dragdrop-values[data-ec=" + ec +"]").val(items.join(','));
    
    if (Drupal.settings.field_test[ec] != -1) {
      if (items.length > Drupal.settings.field_test[ec]) {
        $(".entityreference-dragdrop-message[data-ec=" + ec + "]").show();
        $(".entityreference-dragdrop-selected[data-ec=" + ec + "]").css("border", "1px solid red");
      }
      else {
        $(".entityreference-dragdrop-message[data-ec=" + ec + "]").hide();
        $(".entityreference-dragdrop-selected[data-ec=" + ec + "]").css("border", "");
      }
    }
  };
  
  Drupal.behaviors.field_test = {
    attach: function() {
      
      var $avail = $(".entityreference-dragdrop-available"),
        $select = $(".entityreference-dragdrop-selected");
      
      var $elem_nav = $(".entityreference-dragdrop-navigation-enfants [data-nid]");
      $elem_nav.once().click(function() {
    	  	  var $cont_top = $(this).closest("div.entityreference-dragdrop-container-available");
    	  	  var bundle = $cont_top.find("[data-bundle]").attr("data-bundle");
    	  	  var field_name = $cont_top.find("[data-field_name]").attr("data-field_name");
    	  	  var data_ec = $cont_top.find("[data-ec-var]").attr("data-ec-var");
	    	  var $cont = $(this).closest(".entityreference-dragdrop-navigation-enfants");
	    	  var $avaliable = $cont.closest("div.entityreference-dragdrop-container-available").find("ul.entityreference-dragdrop-available");
	    	  var idInit = $cont.find("[data-nid-init]").attr("data-nid-init");
	          var idReq = $(this).attr("data-nid");
	          var url = '/filariane/init/' + idInit + '/req/' + idReq;
	          $cont.reloadAriane(url);
	          var url3 = "/ajax/field_test/" + bundle + "/" + field_name + "/" + data_ec + "/idReq/" + idReq + "/init/" + idInit;
	          $avaliable.reloadavaliable(url3);
	  });


      $avail.once('entityreference-dragdrop', function () {
        var ec = $(this).data('ec');
        $(this).sortable({
          connectWith: 'ul.entityreference-dragdrop[data-ec=' + ec + ']'
        });
      });

      $select.once('entityreference-dragdrop', function() {
        var ec = $(this).data('ec');
        $(this).sortable({
          connectWith: "ul.entityreference-dragdrop[data-ec=" + ec + ']',
          update: Drupal.field_test.update
        });
      });

      $('.entityreference-dragdrop-filter').once('entityreference-dragdrop-filter', function(){
        $(this).bind('keyup paste', function(){
          var $this = $(this);
          var val = $this.val().toLowerCase();
          if (val != '') {
            $this.parents('.entityreference-dragdrop-container').find('li').each(function(i, elem){
              var $elem = $(elem);
              if ($elem.data('label').toLowerCase().indexOf(val) >= 0) {
                $elem.show();
              }
              else {
                $elem.hide();
              }
            });
          }
          else {
            $this.parents('.entityreference-dragdrop-container').find('li').show();
          }
        });
      });
    }
  };
  
  
  $.fn.reloadAriane = function(url){
	  var div = $(this);
	  	$.ajax({
	  		url: url,
	  		dataType: 'json',
	  		listeDiv: div,
	  		success: function(data){
	  		  div.html(data);
	  		  Drupal.attachBehaviors(div);
	  		},
		});
  }
  
  $.fn.reloadavaliable = function(url){
	  var div = $(this);
	  	$.ajax({
	  		url: url,
	  		dataType: 'json',
	  		listeDiv: div,
	  		success: function(data){
	  		  div.replaceWith(data);
	  		  Drupal.attachBehaviors(div);
	  		},
		});
  }
})(jQuery);
