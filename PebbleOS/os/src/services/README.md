# Services

More concrete documentation.

## Evan Stoddards Thoughts

I'm not sure I like how the original code base used the term "Service".  Especially due to the confusion with BLE services.  It feels like there was a lot that was put into the services directorys that I would argue are not a service, but rather a subsystem.

For instance, the "settings_file" module, feels like a subsystem.

When I hear "service", I think daemon.  At risk of bringing up systemd, anything that would be managed with systemd, I'd equate to a service.

Now, there are a lot of things in the original codebase's services directory that I feel are valid.  Things like HRM, Music, Text Messaging, Notifications.  Those are all "daemons" that run in the background.

However, things like the app cache, persistent storage, that feels more like a subsystem.

If/when serious work begins on the zephyr port, but smarter people than I, I would recommend taking some time to plan out the folder structure, and define a robust vocabulary.  Since using zephyr, I think there are a lot of ideas in the original codebase that follow a different pattern than zephyr (e.g. task vs thread).  We should translate some of the original terms to a more zephyr-oriented vocabulary.  Services is a very good candidate to prioritize.

At risk of this never getting done, I have used some of the original folder structure and terminology, while substituting out some obvious ones, like task to thread.

