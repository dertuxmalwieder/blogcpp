function toggleClass(el, className) {
    if (!el) return;

    let hasClass = el.classList ? el.classList.contains(className) : new RegExp('\\b'+ className+'\\b').test(el.className);
    if (hasClass) {
        // Remove the class.
        if (el.classList) el.classList.remove(className);
        else el.className = el.className.replace(new RegExp('\\b'+ className+'\\b', 'g'), '');
    }
    else {
        // Add the class.
        if (el.classList) el.classList.add(className);
        else el.className += ' ' + className;
    }
}

document.addEventListener('DOMContentLoaded', function() {
    // Responsive UI:
    document.getElementById("menuLink").addEventListener("click", function(e) {
        e.preventDefault();
        toggleClass(document.getElementById("content"), "active");
    });
});
