let main_context_menu = [
    {
        title: "Node",
        action: "sub",
        w: 100,
        val: [
            {
                title: "Test node",
                action: "fun",
                val: function() {
                    alert("nigga");
                }
            }
        ]
    }
]

if (document.addEventListener) {
    document.addEventListener('contextmenu', function(e) {
        document.getElementById("cm-container").innerHTML = "";
        create_context_menu(actual_m_pos[0], actual_m_pos[1], 50, main_context_menu, "true");

        e.preventDefault();
    }, false);
} else {
    document.attachEvent('oncontextmenu', function() {
        document.getElementById("cm-container").innerHTML = "";
        create_context_menu(actual_m_pos[0], actual_m_pos[1], 50, main_context_menu, "true");

        window.event.returnValue = false;
    });
}