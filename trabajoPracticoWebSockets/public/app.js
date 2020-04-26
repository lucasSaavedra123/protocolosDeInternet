// Default Configuration for Toastr
$(document).ready(function() {
    toastr.options = {
        'closeButton': true,
        'debug': false,
        'newestOnTop': false,
        'progressBar': false,
        'positionClass': 'toast-top-right',
        'preventDuplicates': true,
        'showDuration': '1000',
        'hideDuration': '1000',
        'timeOut': '5000',
        'extendedTimeOut': '1000',
        'showEasing': 'swing',
        'progressBar': true,
        'hideEasing': 'linear',
        'showMethod': 'fadeIn',
        'hideMethod': 'fadeOut',
    }
});

function moveToAdminMainPage(){
    window.location.href = "admin.html";
}

function moveToClientMainPage(){
    window.location.href = "client.html";
}