'use strict';

(function($){
	
    $(document).ready(function() {
    		var pageId = $("#nid_de_la_page").val();
    		$.get('/photos/get/photos/' + parseInt(pageId, 10), null, function(response){
    			var test = $('#chart-container').orgchart({
  	    	      'data' : response,
  	    	      'nodeID': 'id',
  	    	      'nodeContent': 'title'
	  	    	});
var couleur_active = 'red';
    			
    			
    			$( "#liste_des_groupes_fieldset .liste_de_groupe_checkbox:checkbox" ).each(function( index ) {
    				var nid = $( this ).val();
    				var title = $("#" + nid + ":not(.node-tree-disable, .node-tree-current) .title");
      	    		title.data( "toogleColor", { statut: 'off', oldColor: title.css("color") } );
    				if($( this ).is(":checked")){
    					title.data( "toogleColor", { statut: 'on', oldColor: title.css("color") } );
    					title.css({"color" : couleur_active});
    				}else{
    					title.data( "toogleColor", { statut: 'off', oldColor: title.css("color") } );
    				}
    			});
    			
    			$("#liste_des_groupes_fieldset .liste_de_groupe_checkbox:checkbox").on('change', function() {
    				var nid = $( this ).val();
    				var title = $("#" + nid + ":not(.node-tree-disable, .node-tree-current) .title");
    				var toogCol = title.data("toogleColor");
    				if(this.checked) {
    					toogCol.statut = 'on';
    					title.css({"color" : couleur_active});
	   			    }else{
	   			    	toogCol.statut = 'off';
	   			    	title.css({"color" : toogCol.oldColor});
	   			    }
    			});
    			
    			test.on('click', '.node:not(.node-tree-disable, .node-tree-current)', function() {
    				
    				//partie toogle
    	    		var title = $( this ).children(".title");
    	    		if (typeof title.data( "toogleColor" ) === 'undefined') {
    	    		  title.data( "toogleColor", { statut: 'off', oldColor: title.css("color") } );
    	    		}
    	    		var toogCol = title.data("toogleColor");
    	    		
    	    		
    	    		if(toogCol.statut=='on'){
    	    			toogCol.statut = 'off';
    	    			title.css({"color" : toogCol.oldColor});
    	    		}else{
    	    			toogCol.statut = 'on';
    	    			title.css({"color" : couleur_active});
    	    		}
    	    		
    	    		var id = $( this ).attr('id');
    				if(id){
    					if(toogCol.statut == 'on'){
    						$("input[name='les_transversalise[" + id + "]']").prop('checked', true);
    					}else{
    						$("input[name='les_transversalise[" + id + "]']").prop('checked', false);
    					}
    				}
    			});
    		});
	    	

    			
    });
})(jQuery);