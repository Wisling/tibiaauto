2.40.0 3.31.2014
Enhancements:
 Tibia Auto: updated for Tibia 10.38
 Seller: Added option to suggest how much banker should withdraw based on items needing to be purchased.
 Banker: Changed the "Withdraw up to keep on hand" to "Always have Keep on Hand" and removed dependence on seller.
 Seller, Banker: Added 5 second timeout after banking or selling before starting again.
 Cavebot: Loot from floor constantly looks for items to pick up that are <= 1 sqm.
 Login: Does not keep trying to log in after wrong password is entered.
 Map Research: Movable, blocking objects no longer cause mapper to research them as blocking unless they are in the way.
 
Defects:
 Login: Made backpack opener work only for non-premium accounts(for now until I can allow users to minimize premium ad windows)
 Auto Go/Log: Character not moved alarm only activates when online.
 Seller, Banker: No longer stops by these if it has nothing to do there.
 Cavebot: Fixed 'Loot from floor' bug that removed certain items from list of looted items.
 Cavebot: Made looting from floor more aggressive, but still avoids being led away with items.
 Cavebot: Fixed bug causing bot to infrequently attack twice in a row quickly.
 Login: Removed lag for logging in when Login window has never been opened.  Status messages start from when window is opened.
 Login: Fixed error with mutex not being freed and holding up all other clients.
 Spellcaster: Added login check to not send spells when not logged in.