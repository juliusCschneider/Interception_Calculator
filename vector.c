#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct {double x; double y;} Vector2D; //struct for position

double get_distance(Vector2D a, Vector2D i){    
    double x = (a.x-i.x);
    double y = (a.x-i.x);
    return sqrt(pow(x, 2) + pow(y,2));
}

double degree_to_rad(double deg){
    return deg*(M_PI/180);
}

double length(Vector2D v){
    return(sqrt(pow(v.x,2) + pow(v.y, 2)));
}

Vector2D get_velocity_vector(double speed, double heading_deg){
    Vector2D v;
    v.x = speed * cos(degree_to_rad(heading_deg));
    v.y = speed * sin(degree_to_rad(heading_deg));
    return v;
}
//Tried to implement my own squre root approximation to save time but then realized
//that the included sqrt() function is not only more precise but also faster (I guess due to lookup tables)
/*
double fast_sqrt(double number){
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *( long *) &y;
    i = 0x5f3759df - ( i >> 1);
    y = * (float *) &i;
    y = y*(threehalfs - ( x2* y * y));
    y = y*(threehalfs - (x2 * y * y));

    y = y*number; //the algo calculates 1/sqrt(n) but we need sqrt(n)

    return y;
}
*/

double dot_product(Vector2D a, Vector2D i){
    return ((a.x*i.x) + (a.y*i.y));
}

double time_to_impact(Vector2D D, Vector2D Vt, double Vi_speed){
    double A = dot_product(Vt, Vt) - (Vi_speed*Vi_speed);
    double B = 2*dot_product(D, Vt);
    double C = dot_product(D,D);

    if (fabs(A) < 1e-9) { //falls A = 0, sind die Geschwindigkeiten identisch Gleichung wird zu Bt + C = 0
        if (fabs(B) < 1e-9) return -1.0; // gibt keine Lösung
        double t = -C / B;
        return (t > 0) ? t : -1.0;
    }

    double delta = (B * B) - (4 * A * C);

    if (delta < 0) return -1.0; //root wäre imaginäre Zahl => Keine Lösung

    double t1 = (-B - sqrt(delta)) / (2 * A);
    double t2 = (-B + sqrt(delta)) / (2 * A);

    //wir nehmen kleinstmöglichste Zahl zur interception.
    if (t1 < 0 && t2 < 0) return -1.0;
    if (t1 < 0) return t2;
    if (t2 < 0) return t1;
    return (t1 < t2) ? t1 : t2;
}

int main(){
    printf("--- KINETIC INTERCEPTION SOLUTION CALCULATOR ---\n");

    double authorization = 0;
    Vector2D target_pos, interceptor_pos;
    double target_speed, target_heading;
    double interceptor_speed; //wir gehen hier von konstanter Geschwindigkeit aus.

    printf("Enter Target Start X Y: ");
    if(scanf("%lf %lf", &target_pos.x, &target_pos.y) != 2) return 1;

    printf("Enter Target Speed and Heading (deg): ");
    if(scanf("%lf %lf", &target_speed, &target_heading) != 2) return 1;

    printf("Enter Interceptor Start X Y: ");
    if(scanf("%lf %lf", &interceptor_pos.x, &interceptor_pos.y) != 2) return 1;

    printf("Enter Interceptor Max Speed: ");
    if(scanf("%lf", &interceptor_speed) != 1) return 1;

    printf("Calculation autorized? Y/N\n");

    char auth = 78; //ACII Code for "N"
    scanf("%s", &auth);
    if(auth == 89){ // ASCII Code for "Y"
        authorization = 1;
        printf("Starting Calculations\n");
    } else {
        authorization = 0;
    }

    if(!authorization){
        printf("Calculation not Authorized\n");
        return 1;
    }

    Vector2D target_vel = get_velocity_vector(target_speed, target_heading);

    Vector2D displacement;
    displacement.x = target_pos.x - interceptor_pos.x;
    displacement.y = target_pos.y - interceptor_pos.y;

    double t = time_to_impact(displacement, target_vel, interceptor_speed);

    if (t < 0) {
        printf("\n[ERROR] Target Out of Range or Unreachable.\n");
    } else {
        printf("\n[SUCCESS] Interception calculated at T = %.4f seconds\n", t);

        Vector2D impact;
        impact.x = target_pos.x + (target_vel.x * t);
        impact.y = target_pos.y + (target_vel.y * t);
        
        printf("Impact Point: (%.2f, %.2f)\n", impact.x, impact.y);

        double dy = impact.y - interceptor_pos.y;
        double dx = impact.x - interceptor_pos.x;
        double launch_rad = atan2(dy, dx);
        double launch_deg = launch_rad * (180.0 / M_PI);

        printf(">>> COMMAND: LAUNCH AT HEADING %.2f DEGREES <<<\n", launch_deg);
    }

    return 0;
}

