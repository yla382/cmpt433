// var tempo = $("#tempoid").val();
const MAX_VOLUME = 100;
const MIN_VOLUME = 0;
const MAX_TEMPO = 300;
const MIN_TEMPO = 40;
var update_send = 0;
var update_rcv = 0;
var dissconnect_error = 0;

var socket = io.connect();

// displays error if connection to bbg is lost
setInterval(function() {
    if (dissconnect_error < 1) {
        socket.emit('REQUEST', "UPDATE");
        update_send = update_send + 1;
        if ((update_send - update_rcv) > 3 && update_send < 10) {
            document.getElementById("error-box").style.display = "block";
            document.getElementById("error-text").innerText = "SERVER ERROR: No response from beat-box application";
        } else{
            document.getElementById("error-box").style.display = "none";
        }
    } else {
        if (dissconnect_error > 9) {
            document.getElementById("error-box").style.display = "none";
        }
        dissconnect_error = dissconnect_error + 1;
    }
    
}, 900);

// display error if connection to node server is lost
socket.on("disconnect", function() {
    dissconnect_error = 1;
    document.getElementById("error-box").style.display = "block";
    document.getElementById("error-text").innerText = "SERVER ERROR: No response from node server";
});

// initial connection to node server
socket.on("connect", function() {
    dissconnect_error = 0;
});

$(document).ready(function() {
    
    // button clicks handlers
    // sends request to node server to be sent to bbg server
     
    $('#turnOffBeatBox').click(function() {
        socket.emit('REQUEST', "QUIT");
        alert("BeatBox program has Terminated");
        $(this).hide();
    });

    $("#volumeUp").click(function() {
        let volume = Number($("#volumeid").val());
        if (volume + 5 <= MAX_VOLUME) {
            $("#volumeid").val(volume + 5);
            socket.emit('REQUEST', "VOLUMN_UP");
        }
        
    });

    $("#volumeDown").click(function() {
        let volume = Number($("#volumeid").val());
        if (volume -5  >= MIN_VOLUME) {
            $("#volumeid").val(volume - 5);
            socket.emit('REQUEST', "VOLUMN_DOWN");
        }

    });

    $("#tempoUp").click(function() {
        let tempo = Number($("#tempoid").val());
        if (tempo + 5 <= MAX_TEMPO) {
            $("#tempoid").val(tempo + 5);
            socket.emit('REQUEST', "TEMPO_UP");
        }
        
    });

    $("#tempoDown").click(function() {
        let tempo = Number($("#tempoid").val());
        if (tempo - 5 >= MIN_TEMPO) {
            $("#tempoid").val(tempo - 5);
            socket.emit('REQUEST', "TEMPO_DOWN");
        }
        
    });


    // DRUM BEAT SELECTION
    $("#modeNone").click(function() {
        socket.emit('REQUEST', "NONE");
    });

    $("#modeRock1").click(function() {
        socket.emit('REQUEST', "ROCK_1");
    });

    $("#modeRock2").click(function() {
        socket.emit('REQUEST', "ROCK_2");
    });


    // PLAY SOUNDS
    $("#playHihat").click(function() {
        socket.emit('REQUEST', "HI_HAT");
    });

    $("#playSnare").click(function() {
        socket.emit('REQUEST', "SNARE");
    });

    $("#playBase").click(function() {
        socket.emit('REQUEST', "BASE");
    });
    
    // handles message received from bbg
    // updates values
    socket.on('commandReply', function(result) {
        update_send = 0;
        update_rcv = 0;
        console.log(result)
        const commands = result.split(",");
        if (commands[0] == "update") {
            $("#volumeid").val(commands[1]);
            $("#tempoid").val(commands[2]);
            $("#status span").text(commands[3]);
            $('#turnOffBeatBox').show();
        }
    });
});
