

var ragMap = new Map();

function define_targets() {
    let l = [];
    PROJECT.nodes.forEach(node => {
        if (node?.ninf?.flag == "target") {
            l.push(node);
        }
    });

    return l;
}


/**
 *  Formalized pipeline;
 

#nodes
uuid;nodeid
. . .

#links
uuid1.interfaceid;uuid2.interfaceid
...
 
#starters
uuid1
uuid2
...
  
  
 */
function formalize() {
    let ret = {};
    let op = "";

    let targets = define_targets();
    let nudl = new Set();

    targets.forEach(target => {
        rec_comp_m(nudl, 0, target);
    });

    op += "#uprefs\n";
    op += `using ${PROJECT.using.join(" ")}\n`

    op += "#nodes\n";
    nudl.forEach((node) => {op += `${node.uuid};${node.id}\n`});
    
    op += "#links\n";
    nudl.forEach((node) => {node.links.forEach((link) => {
        if (link.type == "output") {
            link.con.forEach((con) => {
                let elecom = eleidReverseMap.get(con);
                op+=`${link.parent}.${link.unid};${elecom.parent}.${elecom.unid}\n`;
            });
        }
    })});

    op += "#starters\n";
    nudl.forEach((node) => {
        let flag = false;
            node.links.forEach((lnk) => {
                if (lnk.type == "input" && lnk.con != null) {
                    flag = true;
                }
            });
            if (flag) return;

        op+=`${node.uuid}\n`
    });

    ret.main_line = op;

    let request_prefs_body = {};
    nudl.forEach((node) => {
        if (Object.keys(node.prefs).length > 0) {
            request_prefs_body[node.uuid] = node.prefs;
        }
    })

    ret.rq_body = request_prefs_body;
    return ret;
}

function request_build(rtexec, sendback) {
    let ret = formalize();
    ret["rtexec"] = rtexec;
    ret["sendback"] = sendback;
    fetch("%prod_addr%/build", {
        "headers": {
            "Content-Type": "application/json"
        },
        "body": JSON.stringify(ret),
        "method": "POST"
    }).then((asw) => {
        
        asw.json().then((obj) => {
            if (obj.verdict != 1) {
                console.log("Something fucked up...")
                console.log(obj)
                return
            }
            console.log("Connecting to the websocket session of this shit");
            connect_streamsocket(obj.key)
        });
    });
}

function just_build() {
    request_build(false, true);
}

function runtime_work(sendback) {
    // If allowed
    request_build(true, sendback);
}


// Test one formatter
function trycomp() {
    ragMap.clear();
    let targets = define_targets();

    let layers = [];
    let nudl = new Set();

    targets.forEach(target => {
        rec_comp_m(nudl, 0, target);
    });

    let done = new Set();
    
    let step = 0;
    while (nudl.size > done.size) {
        let future = [];
        nudl.forEach((node) => {
            if (done.has(node)) return;
            let flag = false;
            node.links.forEach((lnk) => {
                if (lnk.type == "input" && lnk.con != null && !done.has(nodeFromLink(lnk.con))) {
                    flag = true;
                }
            });
            if (flag) return;

            future.push(node);            
        });

        future.forEach((node) => {
            done.add(node);
            apt_layer(layers, step,)
        });

        step++;
    }
    
    
    console.log(layers);
}

function apt_layer(layers, i, part) {

    if (layers[i] == undefined) {
        layers[i] = [part];
    } else layers[i].push(part);
}


function rec_comp_m(nudl, indic, node) {
    nudl.add(node);
    node.links.forEach(link => {
            if (link.type != "input") {
                return;
            }

            if (link.con != null) {
                let node =  eleToNodeAddr.get(eleidReverseMap.get(link.con).parent);
                rec_comp_m(nudl, indic+1, node);
            }
        });
}