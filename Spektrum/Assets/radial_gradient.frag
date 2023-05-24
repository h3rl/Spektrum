uniform vec2 windowsize;
uniform float strength; // New parameter for controlling the gradient strength, 0.0025 is fine max

uniform vec3 center_color;
uniform vec3 outer_color;

void main()
{
    // Calculate the distance from the fragment to the center of the circle
    float distance = distance(windowsize.xy * 0.5, gl_FragCoord.xy);
    
    // Calculate the gradient value based on the distance and strength
    float gradient = distance * strength;
    
    // Set the color of the fragment
    gl_FragColor = mix(vec4(center_color, 1.0),vec4(outer_color, 1.0), gradient);
}