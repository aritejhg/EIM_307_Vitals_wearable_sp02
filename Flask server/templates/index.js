
$('select').change(function () {
  if (this.value == "1") {
      $(this).parents('.options').removeClass('stable urgent').addClass('stable');
  } else if (this.value == "2") {
      $(this).parents('.options').removeClass('stable urgent').addClass('urgent');
  } 
});

$('.dd').on('click', function(e){
  e.preventDefault();
    var ct = $('select option[value="2"]:selected').length;
    document.querySelector("#output").innerHTML=ct
    
})