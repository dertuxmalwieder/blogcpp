/**
 * This is an example plug-in for content
 * processing in BlogC++.
 */

var HelloWorldPlugin = {};

HelloWorldPlugin.process = function (input) {
    // The process(string) method is called for every
    // content before writing into the HTML files.
    // Here, we just replace everything by a static
    // text.
    var engine = new BlogEngine();
    return "Hello World from BlogC++ version " + engine.getVersion() + "!";
};

HelloWorldPlugin;
