
var actual_m_pos = [0, 0]
var deltas = [0, 0]

document.body.onmousemove = function (event) {
    deltas = [event.clientX - actual_m_pos[0], event.clientY - actual_m_pos[1]]
    actual_m_pos = [event.clientX, event.clientY]
}

document.body.onmouseleave = function (event) {
    let eles = document.getElementsByClassName("dwindow");
    for (let i = 0; i < eles.length; i++) {
        eles[i].setAttribute("mstate", "false");
    }

    eles = document.getElementsByClassName("dnode");
    for (let i = 0; i < eles.length; i++) {
        eles[i].setAttribute("mstate", "false");
    }
}

document.body.onclick = function (event) {
    let eles = document.getElementsByClassName("dcontext");
    for (let i = 0; i < eles.length; i++) {
        eles[i].remove();
    }
}



/*
    This is probably unnecessary but who cares
*/
// setInterval(() => {
//     let eles = document.getElementsByClassName("dcontext");
//     for (let i = 0; i < eles.length; i++) {
//         if (eles[i].getAttribute("stable") == "false") {
//             if (check___ex(dcontext_mapper[eles[i]])) {
//                 eles[i].remove();
//             }
//         }
//     }
// }, 10);





setInterval(() => {
    let eles = document.getElementsByClassName("dwindow");
    for (let i = 0; i < eles.length; i++) {
        if (eles[i].getAttribute("mstate") == "true") {
            //console.log(parseInt(eles[i].style.left) + deltas[0])
            eles[i].style.left = `${parseInt(eles[i].style.left) + deltas[0]}px`;
            eles[i].style.top = `${parseInt(eles[i].style.top) + deltas[1]}px`;
        }
    }

    eles = document.getElementsByClassName("dnode");
    for (let i = 0; i < eles.length; i++) {
        if (eles[i].getAttribute("mstate") == "true") {
            //console.log(parseInt(eles[i].style.left) + deltas[0])
            eles[i].style.left = `${parseInt(eles[i].style.left) + deltas[0]}px`;
            eles[i].style.top = `${parseInt(eles[i].style.top) + deltas[1]}px`;
        }
    }

    deltas = [0, 0];
}, 1);


document.getElementById("drawer").onclick = function(event) {
    SELECTED_LINK = null;
}

setInterval(() => {
    var canvas=document.getElementById("drawer");
    var ctx=canvas.getContext("2d");
    canvas.width = canvas.getBoundingClientRect().width;
    canvas.height = canvas.getBoundingClientRect().height;
    ctx.clearRect(0,0,3000,3000);


    if (SELECTED_LINK != null) {
        let ele = document.getElementById(SELECTED_LINK.eleid);

        let dims = ele.getBoundingClientRect()

        ctx.beginPath();
        ctx.moveTo(dims.left+dims.width/2,dims.top+dims.height/2);
        ctx.lineTo(actual_m_pos[0], actual_m_pos[1]);
        ctx.lineWidth=2;
        ctx.strokeStyle = "red";
        ctx.stroke();

        console.log("WORKING!!!")
    }

    PROJECT.nodes.forEach(v => {
        v.links.forEach(lk => {
            if (lk.type == "input") return;
            let dims1 = document.getElementById(lk.eleid).getBoundingClientRect();
            lk.con.forEach((con) => {
                let dims2 = document.getElementById(con).getBoundingClientRect();

                ctx.beginPath();
                ctx.moveTo(dims1.left+dims1.width/2,dims1.top+dims1.height/2);
                ctx.lineTo(dims2.left+dims2.width/2,dims2.top+dims2.height/2);
                ctx.lineWidth=4;
                ctx.strokeStyle = `#${TYPELESS.get(lk.dtype).color}`;
                ctx.stroke();
            });
        });
    });
}, 100);