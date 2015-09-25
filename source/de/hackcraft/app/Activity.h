/* 
 * File:     Activity.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on December 14, 2013, 4:44 PM
 */

#ifndef ACTIVITY_H
#define	ACTIVITY_H

class Activity {
    friend class Application;
    
public:
    Activity();
    Activity(const Activity& orig);
    virtual ~Activity();
    
protected:
    void onCreate() {};
    void onStart() {};
    void onRestart() {};
    void onResume() {};
    void onPause() {};
    void onStop() {};
    void onDestroy() {};
    
private:

};

#endif	/* ACTIVITY_H */

