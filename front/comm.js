//
//  PROTOTYPE COMMUNICATION FILE
//
var ComSocket = null;

function disconnect_streamsocket() {
    ComSocket.disconnect();
    ComSocket = null;
}

function connect_streamsocket(key) {
    ComSocket = io.connect(`%prod_addr%`)

    // Connection logic
    // ComSocket.on('connect', function() {
    //     ComSocket.emit('stab', {data: 'Client connected!'});
    // });



    ComSocket.on('linkrq', function(msgin) {
        ComSocket.emit('linkaw', {key: key});
        console.log("PODKL_GOG");
    });

    ComSocket.on('linkres', function(msgin) {
        if (msgin.result != 'OCK') disconnect_streamsocket();
        else console.log("PODKL_SUS");
    });

    ComSocket.on('byebye', function(msgin) {
        disconnect_streamsocket();
    });

    ComSocket.on('build_ready', function(msgin) {
        // File download thingy here
        console.log("Download ready");
    });
}


