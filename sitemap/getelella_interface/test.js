(function ($) {
  Drupal.behaviors.codeexpertz = {
    attach: function (context, settings) {
      $('input[type=checkbox][data-toggle^=toggle]').bootstrapToggle();
      $getelella_interval = null;
      //test @test
      /*$("#liste_message_discussion").on('click', function(){
    	  CKEDITOR.instances['edit-message-value'].setData("my textstring");
      });*/
      
      $('#reload-form').once().on('click', function(){
    	  $('#reload-form').reinitMediaRecorder();
    	  
    	  
      });
      var nid_forum_page = $(":input[name=nid_forum_page]").val();
      var liste_discussion = $("#liste_discussion");
      liste_discussion.data("nid_forum", nid_forum_page);
      liste_discussion.once().srollerVers();
      if(!window.rafraichirNonLus){
	  		var interval = setInterval(
	  			function(liste_discussion){
	  				liste_discussion.once().rafraichirNumerosNonLus(nid_forum_page);
	  			}, 
	  			20000, 
	  			liste_discussion
	  		);
	  		window.rafraichirNonLus = interval;
	  		liste_discussion.once().rafraichirNumerosNonLus(nid_forum_page);
      }
      
      $('a.testeee').once().on('click', function(){
    	  var div = $("#liste_message_discussion");
    	  div.html('<div class="loader"></div>');
    	  div.stopRefreshMessage();
    	  //$("#form-wrapper").hide();
    	  var nid_forum = $(this).children(":input[name=forum_id]").val();
    	  $('#form-wrapper :input[name=nid_forum][type=text]').val(nid_forum);
    	  div.data("scrollable", false);
    	  div.data("scrollended", false);
    	  div.data("nid_forum", nid_forum);
    	  div.refresh_discussion_message_list();
    	  
    	  div.set_status_viewed_discusion(nid_forum);

    	  
    	  
    	  $("#form-wrapper").show();
    	  div.startRefreshMessage();
    	  $("#liste_discussion").find(".nbr-non-lu").text("0");
      });
      

      var gadge = 20;
      
      $("#liste_message_discussion").on( "scroll", function(){
    	  var scrollended = $(this).data("scrollended");
    	  if($(this).scrollTop() <= gadge && $(this).data("scrollable") && !scrollended){
    		  $(this).data("scrollable", false);
    		  var nid_forum = $(this).data("nid_forum");
    		  $(this).refresh_discussion_message_list('premier');
    	  }
      });
      
      
      liste_discussion.on( "scroll", function(){
    	  var scrollended = $(this).data("scrollended");
    	  if($(this).scrollTop() <= gadge && $(this).data("scrollable") && !scrollended){
    		  $(this).data("scrollable", false);
    		  $(this).refresh_discussion_list('premier');
    	  }
      });
      
      
      $('#pager ul.pagination li a').click(function(event) {
    	event.preventDefault();
        thisLink = $(this);
        thisUrl = this.href;
        target = $("#node_list");
        
        var url = new URL(thisUrl);
        var c = url.searchParams.get("page");
        if(c == null){
        	c = 0;
        }
        $.ajax({
          url: 'ajax/get/vuelist/0?page=' + c,
          type: "GET",
          success: function(data) {
            $('.content').html(data);
            Drupal.attachBehaviors("#pager");
          }
        });
        return false;
      });
      
      
    }
  };
  
  $.fn.startRefreshMessage = function(){
	  	var div = $(this);
		var interval = setInterval(
			function(div){
				div.refresh_discussion_message_list('dernier');
			}, 
			3000, 
			div
		);
	    div.data("interval", interval);
  }
  
  $.fn.stopRefreshMessage = function(){
	  if($(this).data("interval") !== undefined){
		  clearInterval($(this).data("interval"));
	  }
  }
  
  $.fn.rafraichirNumerosNonLus = function(nid_forum){
	  //var nid_forum = $(this).data("nid_forum");
	  $.each( $(".item-discussion-forum"), function( key, value ) {
		  var nid = $(this).find("a :input[name=forum_id]").val();
		  var texte = $(this).find(".nbr-non-lu").text();
		  
		  
		  var url = '/ajax/count/liste/message/' + nid;
		  $.ajax({
		  		url: url,
		  		type: "GET",
		  		dataType: 'json',
		  		divUse: $(this),
		  		success: function(data){
		  			this.divUse.find(".nbr-non-lu").text(data);
		  		},
		  });
		  
		  
	  });
  }
  
  $.fn.srollerVers = function(hauteurAvant){
		var hauteurApres = $(this).prop('scrollHeight');
		if(hauteurAvant !== undefined){
			$(this).data("scrollable", false).scrollTop(hauteurApres - hauteurAvant).data("scrollable", true);
			if($(this).find(":input[name=liste_termine]").val() == "true"){
				$(this).data("scrollended", true);
			}
		}else{
			$(this).data("scrollable", false).scrollTop(hauteurApres).data( "scrollable", true );
		}
  }
  
  $.fn.set_status_viewed_discusion = function(nidDiscussion){
	  var url = '/ajax/set/message/viewed/' + nidDiscussion;
	  $.ajax({
	  		url: url,
	  		type: "GET",
	  		dataType: 'json',
	  		success: function(data){
	  		},
		});
  }
 
  $.fn.refresh_discussion_message_list = function(position, exclusive) {
	  if(exclusive || $(this).data("stop") != true){
		  // eclusive et data stop permettent d'éviter que la liste de message se recharge de manière chavauchée
		  if(exclusive){
			  $(this).data("stop", true);
		  }
		  var nid_forum = $(this).data("nid_forum");
		  var timestamp;
		  var url;
		  if(position == 'premier'){
			  timestamp = $(this).find(":input[name=premier_timestamp]:first").val();
			  url = '/ajax/get/liste/message/' + nid_forum + "/premier/" + timestamp;
		  }else if(position == 'dernier'){
			  timestamp = $(this).find(":input[name=dernier_timestamp]:last").val();
			  url = '/ajax/get/liste/message/' + nid_forum + "/dernier/" + timestamp;
		  }else{
			  url = '/ajax/get/liste/message/' + nid_forum; 
		  }
		  $(this).faireEnAjax(url, exclusive, position);
	  }
  };
  
  $.fn.faireEnAjax = function(url, exclusive, position, test){
	  var div = $(this);
	  	$.ajax({
	  		url: url,
	  		dataType: 'json',
	  		listeDiv: div,
	  		test: test,
	  		exclusive: exclusive,
	  		position: position,
	  		success: function(data){
	  		  var div = this.listeDiv;
	  		  var hauteurAvant = div.prop('scrollHeight');
	  		  if(this.position == 'premier'){
	  			div.prepend(data).srollerVers(hauteurAvant);
	  			if(div.find(":input[name=liste_termine]").val() == "true"){
  					div.data("scrollended", true);
  				}
	  		  }else if(this.position == 'dernier'){
	  			if(data != 'empty'){
	  				div.append(data).srollerVers();
	  			}
	  		  }else{
	  			div.append(data);
	  			var hauteurApres = $(this).prop('scrollHeight');
	  			div.srollerVers();
	  			/*div.fadeOut( "slow", function() {
	  				div.append(data).fadeIn('slow', function(){
	  					var hauteurApres = $(this).prop('scrollHeight');
	  					div.srollerVers();
	  				});
			  	});*/
	  		  }
	  		  Drupal.attachBehaviors(this);
	  		  div.data("stop", false);
	  		  if(div.find(".loader") !== undefined){
	  			div.find(".loader").remove();
	  		  }
	  		},
		});
  }
  
  $.fn.reinitMediaRecorder = function(){
	  $.each(Drupal.settings.mediaRecorder.elements, function (key, info) {
          var $mediaRecorder = $("#" + info.id);
          var $video = $mediaRecorder.find('.media-recorder-video');
          var $previewWrapper = $mediaRecorder.find('.media-recorder-preview');
          var $input = $(':input[name=field_sku\\[und\\]\\[0\\]\\[fid\\]]');
	      	//alert("$mediaRecorder.html():" + $mediaRecorder.html());
	      	//alert("$input:" + $input + " val:" + $input.val() + $input.html());
	      	//alert("$video.html():" + $video[0].src);
	      	$previewWrapper.hide();
	      	$video[0].src = "";
	      	$input.val("0");
	      	//$mediaRecorder.trigger('recordStop');
	  });
  }
  
  
  $.fn.refresh_discussion_list = function(position, exclusive) {
	  if(exclusive || $(this).data("stop") != true){
		  // eclusive et data stop permettent d'éviter que la liste de message se recharge de manière chavauchée
		  if(exclusive){
			  $(this).data("stop", true);
		  }
		  var nid_forum = $(this).data("nid_forum");
		  var timestamp;
		  var url;
		  if(position == 'premier'){
			  timestamp = $(this).find(":input[name=premier_timestamp]:first").val();
			  url = '/ajax/get/liste/discussion/' + nid_forum + "/premier/" + timestamp;
		  }else if(position == 'dernier'){
			  timestamp = $(this).find(":input[name=dernier_timestamp]:last").val();
			  url = '/ajax/get/liste/discussion/' + nid_forum + "/dernier/" + timestamp;
		  }else{
			  url = '/ajax/get/liste/discussion/' + nid_forum; 
		  }
		  $(this).faireEnAjax(url, exclusive, position, true);
	  }
  }
})(jQuery);




	/*$.get(url,function(data){
  var div = $( "#liste_message_discussion" );
  var hauteurAvant = div.prop('scrollHeight');
  if(position == 'premier'){
	div.prepend(data).srollerVers(hauteurAvant);
  }else if(position == 'dernier'){
	div.append(data).srollerVers();
  }else{
	div.fadeOut( "slow", function() {
		$(this).append(data).fadeIn('slow', function(){
			var hauteurApres = $(this).prop('scrollHeight');
			$(this).srollerVers();
		});
	});
  }
  Drupal.attachBehaviors(this);
});*/

