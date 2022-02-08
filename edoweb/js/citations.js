(function($) {
    var DOI = "1234/567abc";
    var badge = $("#badge")

    $('#doi').text("hallo");
    console.log(DOI);
    badge.attr("data-doi", DOI);
    console.log(badge);

    var metadata = 'https://api.ellinet-dev.hbz-nrw.de/resource/frl:6402838.json2';

    $.getJSON(metadata, function( data ) {
        //console.log("doi:" + data.bibo:doi);
        console.log(data);
        console.log(data["bibo:doi"][0]);
    });


})(jQuery);

