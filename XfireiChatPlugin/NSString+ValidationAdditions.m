//
//  NSString+ValidationAdditions.m
//  Xblaze-iPhone
//
//  Created by James Addyman on 13/11/2009.
//  Copyright 2009 JamSoft. All rights reserved.
//

#import "NSString+ValidationAdditions.h"


@implementation NSString (ValidationAdditions)

- (BOOL)hasText
{
	BOOL hasText = YES;
	
	if ([self isEqualToString:@""])
	{
		hasText = NO;
	}
	
	return hasText;
}

@end
