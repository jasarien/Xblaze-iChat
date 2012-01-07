/*******************************************************************
	FILE:		XfireFriend_MacFireAdditions.h
	
	COPYRIGHT:
		Copyright 2008, the MacFire.org team.
		Use of this software is governed by the license terms
		indicated in the License.txt file (a BSD license).
	
	DESCRIPTION:
		Additions to the XfireFriend class for use by the UI.  This
		purposefully splits UI code from Xfire library code.
	
	HISTORY:
		2008 04 27  Added sorting method.
		2008 04 12  Created.
*******************************************************************/

#import <Cocoa/Cocoa.h>

#import "XfireFriend.h"

@interface XfireFriend (MacFireAdditions)

- (NSImage *)displayImage;
- (NSString *)statusDisplayString;

@end
